#include <ase/player/systems/network/player_net_broadcast_system.hpp>
#include <ase/player/components/state/player_state_id_component.hpp>
#include <ase/player/components/state/player_state_pos_component.hpp>
#include <ase/player/components/state/player_state_status_component.hpp>
#include <ase/player/components/state/player_state_cfg_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/components/tag/player_tag_chunk_changed_component.hpp>
#include <ase/replication/components/replication_message.hpp>
#include <ase/log/log.hpp>

#include <nlohmann/json.hpp>

namespace ase::player {

void PlayerNetBroadcastSystem::on_start(ecs::Registry& /*registry*/) {
}

void PlayerNetBroadcastSystem::on_stop(ecs::Registry& /*registry*/) {
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

            auto msg = registry.create();
            registry.emplace<replication::ReplicationData>(msg, "player_spawn", data.dump());
            registry.emplace<replication::PendingBroadcast>(msg);

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

            auto msg = registry.create();
            registry.emplace<replication::ReplicationData>(msg, "player_update", data.dump());
            registry.emplace<replication::PendingBroadcast>(msg);

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
