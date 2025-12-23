#include <ase/player/systems/network/player_net_bct_system.hpp>
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/components/tag/player_tag_chunk_changed_component.hpp>
#include <ase/replication/replication.hpp>
#include <ase/log/log.hpp>

#include <nlohmann/json.hpp>
#include <cstring>

namespace ase::player {

namespace {

// =============================================================================
// HELPER: Create broadcast message (to all clients)
// =============================================================================

void create_broadcast_message(
    ecs::Registry& registry,
    const char* channel,
    const std::string& payload
) {
    auto msg = registry.create();
    auto& dat = registry.emplace<replication::ReplicationMsgDatComponent>(msg);

    std::strncpy(dat.chn.data(), channel, dat.chn.size() - 1);
    dat.chn[dat.chn.size() - 1] = '\0';

    auto* pay = new char[payload.size()];
    std::memcpy(pay, payload.data(), payload.size());
    dat.pay_ptr = reinterpret_cast<uint64_t>(pay);
    dat.pay_len = static_cast<uint32_t>(payload.size());

    registry.emplace<replication::ReplicationMsgPndBctTag>(msg);
}

// =============================================================================
// HELPER: Serialize player state in codegen-expected format
// =============================================================================

nlohmann::json serialize_player_id(const PlayerStIdComponent& id) {
    using namespace std::chrono;
    auto spawned_ms = duration_cast<milliseconds>(id.spawned_at.time_since_epoch()).count();
    auto input_ms = duration_cast<milliseconds>(id.last_input.time_since_epoch()).count();
    return {
        {"player_id", id.player_id},
        {"spawned_at", static_cast<double>(spawned_ms)},
        {"last_input", static_cast<double>(input_ms)}
    };
}

nlohmann::json serialize_player_pos(const PlayerStPosComponent& pos) {
    return {
        {"x", pos.x},
        {"y", pos.y},
        {"z", pos.z},
        {"yaw", pos.yaw}
    };
}

nlohmann::json serialize_player_vel(const PlayerStVelComponent& vel) {
    return {
        {"vx", vel.vx},
        {"vy", vel.vy},
        {"vz", vel.vz}
    };
}

nlohmann::json serialize_player_sts(const PlayerStStsComponent& sts) {
    return {
        {"state", sts.state}
    };
}

}  // anonymous namespace

// =============================================================================
// LIFECYCLE
// =============================================================================

void PlayerNetBctSystem::on_start(ecs::Registry& /*registry*/) {
    // No resources to initialize
}

void PlayerNetBctSystem::on_stop(ecs::Registry& registry) {
    // Cleanup pending broadcast messages - free allocated payloads
    auto view = registry.view<replication::ReplicationMsgDatComponent, replication::ReplicationMsgPndBctTag>();
    std::vector<ecs::Entity> to_destroy;

    for (auto [entity, msg] : view.each()) {
        if (msg.pay_ptr != 0) {
            delete[] reinterpret_cast<char*>(msg.pay_ptr);
        }
        to_destroy.push_back(entity);
    }

    for (auto entity : to_destroy) {
        registry.destroy(entity);
    }

    if (!to_destroy.empty()) {
        log::info("[PlayerNetBctSystem] Cleaned up {} pending messages on shutdown", to_destroy.size());
    }
}

// =============================================================================
// TICK
// =============================================================================

void PlayerNetBctSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // =========================================================================
    // 1. BROADCAST SPAWNED PLAYERS (reliable channel)
    //    codegen-format: {playerStId: {...}, playerStPos: {...}}
    // =========================================================================
    {
        auto view = registry.view<
            PlayerStIdComponent,
            PlayerStPosComponent,
            PlayerSpawnedTag
        >();

        std::vector<ecs::Entity> to_remove;
        for (auto [entity, id, pos] : view.each()) {
            nlohmann::json data;
            data["playerStId"] = serialize_player_id(id);
            data["playerStPos"] = serialize_player_pos(pos);

            create_broadcast_message(registry, "player_spawn", data.dump());
            log::debug("[PlayerNetBctSystem] Broadcast spawn for player {}", id.player_id.data());
            to_remove.push_back(entity);
        }

        for (auto entity : to_remove) {
            registry.remove<PlayerSpawnedTag>(entity);
        }
    }

    // =========================================================================
    // 2. BROADCAST DIRTY PLAYERS (unreliable channel for fast updates)
    //    codegen-format: {playerStPos: {...}, playerStVel: {...}, playerStSts: {...}}
    // =========================================================================
    {
        auto view = registry.view<
            PlayerStIdComponent,
            PlayerStPosComponent,
            PlayerStVelComponent,
            PlayerStStsComponent,
            PlayerDirtyTag
        >();

        std::vector<ecs::Entity> to_remove;
        for (auto [entity, id, pos, vel, sts] : view.each()) {
            nlohmann::json data;
            data["playerStId"] = serialize_player_id(id);  // Include ID for client to find entity
            data["playerStPos"] = serialize_player_pos(pos);
            data["playerStVel"] = serialize_player_vel(vel);
            data["playerStSts"] = serialize_player_sts(sts);

            create_broadcast_message(registry, "player_state", data.dump());
            to_remove.push_back(entity);
        }

        for (auto entity : to_remove) {
            registry.remove<PlayerDirtyTag>(entity);
        }
    }

    // =========================================================================
    // 3. CLEAR CHUNK CHANGED TAGS
    // =========================================================================
    {
        auto view = registry.view<PlayerChunkChangedTag>();
        std::vector<ecs::Entity> to_remove;
        for (auto entity : view) {
            to_remove.push_back(entity);
        }
        for (auto entity : to_remove) {
            registry.remove<PlayerChunkChangedTag>(entity);
        }
    }
}

}  // namespace ase::player
