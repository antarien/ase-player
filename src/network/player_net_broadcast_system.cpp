#include <ase/player/systems/network/player_net_broadcast_system.hpp>
#include <ase/player/components/state/player_state_id_component.hpp>
#include <ase/player/components/state/player_state_pos_component.hpp>
#include <ase/player/components/state/player_state_status_component.hpp>
#include <ase/player/components/state/player_state_cfg_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/components/tag/player_tag_chunk_changed_component.hpp>
#include <ase/replication/replication.hpp>
#include <ase/log/log.hpp>

#include <nlohmann/json.hpp>
#include <cstring>

namespace ase::player {

namespace {

void create_broadcast_message(
    ecs::Registry& registry,
    const char* channel,
    const std::string& payload
) {
    auto msg = registry.create();
    auto& msg_data = registry.emplace<replication::ReplicationMsgDatComponent>(msg);

    std::strncpy(msg_data.chn.data(), channel, msg_data.chn.size() - 1);
    msg_data.chn[msg_data.chn.size() - 1] = '\0';

    auto* pay = new char[payload.size()];
    std::memcpy(pay, payload.data(), payload.size());
    msg_data.pay_ptr = reinterpret_cast<uint64_t>(pay);
    msg_data.pay_len = static_cast<uint32_t>(payload.size());

    registry.emplace<replication::ReplicationMsgPndBctTag>(msg);
}

}  // anonymous namespace

void PlayerNetBroadcastSystem::on_start(ecs::Registry& /*registry*/) {
}

void PlayerNetBroadcastSystem::on_stop(ecs::Registry& registry) {
    // Cleanup pending broadcast messages - free allocated payloads
    auto view = registry.view<replication::ReplicationMsgDatComponent, replication::ReplicationMsgPndBctTag>();
    std::vector<ecs::Entity> to_destroy;

    for (auto [entity, msg] : view.each()) {
        // Free allocated payload
        if (msg.pay_ptr != 0) {
            delete[] reinterpret_cast<char*>(msg.pay_ptr);
        }
        to_destroy.push_back(entity);
    }

    for (auto entity : to_destroy) {
        registry.destroy(entity);
    }

    if (!to_destroy.empty()) {
        log::info("[PlayerNetBroadcastSystem] Cleaned up {} pending messages on shutdown", to_destroy.size());
    }
}

void PlayerNetBroadcastSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // Broadcast just-spawned players
    {
        auto view = registry.view<
            PlayerStateIdComponent,
            PlayerStatePosComponent,
            PlayerSpawnedTag
        >();

        std::vector<ecs::Entity> to_remove;
        for (auto [entity, identity, pos] : view.each()) {
            nlohmann::json data = {
                {"player_id", identity.player_id},
                {"position", {{"x", pos.x}, {"y", pos.y}, {"z", pos.z}}}
            };

            create_broadcast_message(registry, "player_spawn", data.dump());
            to_remove.push_back(entity);
        }
        for (auto entity : to_remove) {
            registry.remove<PlayerSpawnedTag>(entity);
        }
    }

    // Broadcast dirty players (position updates)
    {
        auto view = registry.view<
            PlayerStateIdComponent,
            PlayerStatePosComponent,
            PlayerStateStatusComponent,
            PlayerDirtyTag
        >();

        std::vector<ecs::Entity> to_remove;
        for (auto [entity, identity, pos, state] : view.each()) {
            nlohmann::json data = {
                {"player_id", identity.player_id},
                {"position", {{"x", pos.x}, {"y", pos.y}, {"z", pos.z}}},
                {"yaw", pos.yaw},
                {"state", static_cast<int>(state.state)}
            };

            create_broadcast_message(registry, "player_update", data.dump());
            to_remove.push_back(entity);
        }
        for (auto entity : to_remove) {
            registry.remove<PlayerDirtyTag>(entity);
        }
    }

    // Clear chunk changed tags
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
