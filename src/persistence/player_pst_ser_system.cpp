#include <ase/player/systems/persistence/player_pst_ser_system.hpp>
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/buffer/player_buf_pst_component.hpp>
#include <ase/player/components/tag/player_tag_pst_dty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/types.hpp>

#include <ase/replication/replication.hpp>
#include <ase/log/log.hpp>

#include <cstring>

namespace ase::player {

namespace {

constexpr char COLLECTION_NAME[] = "players";
constexpr size_t COLLECTION_NAME_LEN = sizeof(COLLECTION_NAME) - 1;
constexpr size_t MAX_PLAYERS_PER_TICK = 50;

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
}

void PlayerPstSerSystem::tick(ecs::Registry& registry, float /*dt*/) {
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

        if (registry.all_of<replication::ReplicationPstMetComponent>(entity)) {
            continue;
        }

        if (id.player_id.empty()) {
            registry.remove<PlayerPstDtyTag>(entity);
            continue;
        }

        // Create persistence buffer
        auto& buf = registry.get_or_emplace<PlayerBufPstComponent>(entity);

        // Free old allocations
        free_string(buf.plr_id_ptr);

        // Store player ID
        buf.plr_id_ptr = alloc_string(id.player_id);
        buf.plr_id_len = static_cast<uint32_t>(id.player_id.size());

        // Create serialization request entity
        auto ser = registry.create();

        // Set up buffer with raw data pointer
        auto& jsn_buf = registry.emplace<serial::SerialBufJsnComponent>(ser);
        jsn_buf.src_ptr = reinterpret_cast<uint64_t>(&pos);
        jsn_buf.src_typ = SERIAL_TYP_PLR_STA;
        jsn_buf.src_siz = sizeof(PlayerStPosComponent);
        jsn_buf.st = serial::SERIAL_ST_PND;

        // Mark for serialization
        registry.emplace<serial::SerialJsnPndTag>(ser);

        // Set Replication Layer metadata
        auto& pst_met = registry.emplace<replication::ReplicationPstMetComponent>(entity);
        pst_met.col_ptr = reinterpret_cast<uint64_t>(COLLECTION_NAME);
        pst_met.col_len = COLLECTION_NAME_LEN;
        pst_met.eid_ptr = buf.plr_id_ptr;
        pst_met.eid_len = buf.plr_id_len;
        pst_met.ser_ent = static_cast<uint32_t>(ser);

        // Mark for Replication Layer processing
        registry.emplace<replication::ReplicationSynTag>(entity);

        // Remove pending persistence tag
        registry.remove<PlayerPstDtyTag>(entity);

        processed++;
        log::debug("[PlayerPstSerSystem] Queued player {} for persistence", id.player_id);
    }
}

void PlayerPstSerSystem::on_stop(ecs::Registry& registry) {
    auto view = registry.view<PlayerBufPstComponent>();
    for (auto [entity, buf] : view.each()) {
        free_string(buf.jsn_ptr);
        free_string(buf.plr_id_ptr);
        buf.jsn_ptr = 0;
        buf.plr_id_ptr = 0;
    }
}

}  // namespace ase::player
