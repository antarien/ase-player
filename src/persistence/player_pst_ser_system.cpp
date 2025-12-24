#include <ase/player/systems/persistence/player_pst_ser_system.hpp>
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/buffer/player_buf_pst_component.hpp>
#include <ase/player/components/tag/player_tag_pst_dty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/types.hpp>

#include <ase/replication/components/persist/replication_pst_met_component.hpp>
#include <ase/replication/components/tag/replication_tag_dty_component.hpp>

#include <ase/log/log.hpp>
#include <nlohmann/json.hpp>

#include <cstring>

namespace ase::player {

namespace {

// Collection name for players (persistent string)
constexpr char COLLECTION_NAME[] = "players";
constexpr size_t COLLECTION_NAME_LEN = sizeof(COLLECTION_NAME) - 1;

constexpr size_t MAX_PLAYERS_PER_TICK = 50;

// Serialize player state to JSON for MongoDB
std::string serialize_player_to_json(
    const std::string& player_id,
    const PlayerStPosComponent& pos,
    const PlayerStVelComponent& vel,
    const PlayerStStsComponent& sts) {

    nlohmann::json doc;

    // Player identification
    doc["_id"] = player_id;

    // Position and rotation
    doc["pos"] = {
        {"x", pos.x},
        {"y", pos.y},
        {"z", pos.z}
    };
    doc["yaw"] = pos.yaw;

    // Velocity
    doc["vel"] = {
        {"x", vel.vx},
        {"y", vel.vy},
        {"z", vel.vz}
    };

    // State
    doc["state"] = sts.state;

    // Timestamp
    doc["updated_at"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    return doc.dump();
}

// Allocate and store string in heap (returns pointer)
uint64_t alloc_string(const std::string& str) {
    if (str.empty()) return 0;
    char* buf = new char[str.size() + 1];
    std::memcpy(buf, str.c_str(), str.size() + 1);
    return reinterpret_cast<uint64_t>(buf);
}

void free_string(uint64_t ptr) {
    if (ptr != 0) {
        delete[] reinterpret_cast<char*>(ptr);
    }
}

}  // anonymous namespace

void PlayerPstSerSystem::on_start(ecs::Registry& /*registry*/) {
    // No resources to initialize
}

void PlayerPstSerSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // Query players marked for persistence
    auto view = registry.view<
        PlayerPstDtyTag,
        PlayerSpawnedTag,
        PlayerStIdComponent,
        PlayerStPosComponent,
        PlayerStVelComponent,
        PlayerStStsComponent
    >();

    size_t processed = 0;

    for (auto [entity, id, pos, vel, sts] : view.each()) {
        if (processed >= MAX_PLAYERS_PER_TICK) break;

        // Skip if already has persistence metadata (avoid duplicate processing)
        if (registry.all_of<replication::ReplicationPstMetComponent>(entity)) {
            continue;
        }

        // Skip if player_id is empty
        if (id.player_id.empty()) {
            registry.remove<PlayerPstDtyTag>(entity);
            continue;
        }

        // Serialize player to JSON
        std::string json_doc = serialize_player_to_json(id.player_id, pos, vel, sts);

        // Create persistence buffer
        auto& buf = registry.get_or_emplace<PlayerBufPstComponent>(entity);

        // Free old allocations if any
        free_string(buf.jsn_ptr);
        free_string(buf.plr_id_ptr);

        // Store new data
        buf.jsn_ptr = alloc_string(json_doc);
        buf.jsn_len = static_cast<uint32_t>(json_doc.size());
        buf.plr_id_ptr = alloc_string(id.player_id);
        buf.plr_id_len = static_cast<uint32_t>(id.player_id.size());
        buf.st = 2;  // Done

        // Set Replication Layer metadata
        auto& pst_met = registry.emplace<replication::ReplicationPstMetComponent>(entity);

        // Pre-serialized JSON (src_typ == 0 means skip ase-serial)
        pst_met.src_ptr = buf.jsn_ptr;
        pst_met.src_typ = 0;  // Already JSON - no serialization needed
        pst_met.src_siz = buf.jsn_len;

        // MongoDB collection name
        pst_met.col_ptr = reinterpret_cast<uint64_t>(COLLECTION_NAME);
        pst_met.col_len = COLLECTION_NAME_LEN;

        // Entity ID for upsert filter (player ID)
        pst_met.eid_ptr = buf.plr_id_ptr;
        pst_met.eid_len = buf.plr_id_len;

        // Mark for Replication Layer processing
        registry.emplace<replication::ReplicationDtyTag>(entity);

        // Remove pending persistence tag
        registry.remove<PlayerPstDtyTag>(entity);

        processed++;
        log::debug("[PlayerPstSerSystem] Queued player {} for persistence", id.player_id);
    }
}

void PlayerPstSerSystem::on_stop(ecs::Registry& registry) {
    // Cleanup allocated buffers
    auto view = registry.view<PlayerBufPstComponent>();
    for (auto [entity, buf] : view.each()) {
        free_string(buf.jsn_ptr);
        free_string(buf.plr_id_ptr);
        buf.jsn_ptr = 0;
        buf.plr_id_ptr = 0;
    }
}

}  // namespace ase::player
