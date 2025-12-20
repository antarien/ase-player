#include <ase/player/systems/player_broadcast_system.hpp>
#include <ase/player/components/player_identity_component.hpp>
#include <ase/player/components/player_position_component.hpp>
#include <ase/player/components/player_state_component.hpp>
#include <ase/player/components/player_config_component.hpp>
#include <ase/player/components/player_tags.hpp>
#include <ase/replication/components/replication_message.hpp>
#include <ase/ecs/schedule_registry.hpp>
#include <ase/log/log.hpp>

#include <nlohmann/json.hpp>

namespace ase::player {

// =============================================================================
// LIFECYCLE
// =============================================================================

void PlayerBroadcastSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerBroadcastSystem] Started");
}

void PlayerBroadcastSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerBroadcastSystem] Stopped");
}

// =============================================================================
// TICK - Broadcast dirty players via ECS message entities
// =============================================================================

void PlayerBroadcastSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // Broadcast just-spawned players
    {
        auto view = registry.view<
            PlayerIdentityComponent,
            PlayerPositionComponent,
            PlayerJustSpawnedTag
        >();

        std::vector<ecs::Entity> to_remove;
        for (auto [entity, identity, pos] : view.each()) {
            nlohmann::json data = {
                {"player_id", identity.player_id},
                {"position", {{"x", pos.x}, {"y", pos.y}, {"z", pos.z}}}
            };

            // Create message entity for broadcast
            auto msg = registry.create();
            registry.emplace<replication::ReplicationData>(msg, "player_spawn", data.dump());
            registry.emplace<replication::PendingBroadcast>(msg);

            to_remove.push_back(entity);
        }
        for (auto entity : to_remove) {
            registry.remove<PlayerJustSpawnedTag>(entity);
        }
    }

    // Broadcast dirty players (position updates)
    {
        auto view = registry.view<
            PlayerIdentityComponent,
            PlayerPositionComponent,
            PlayerStateComponent,
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

            // Create message entity for broadcast
            auto msg = registry.create();
            registry.emplace<replication::ReplicationData>(msg, "player_update", data.dump());
            registry.emplace<replication::PendingBroadcast>(msg);

            to_remove.push_back(entity);
        }
        for (auto entity : to_remove) {
            registry.remove<PlayerDirtyTag>(entity);
        }
    }

    // Broadcast chunk changes (for Neo4j - will be handled by observer system)
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

// =============================================================================
// REGISTRATION
// =============================================================================

REGISTER_SYSTEM(PlayerBroadcastSystem)
    .in_schedule(ecs::Schedule::FixedUpdate)
    .with_priority(75)
    .run_after("PlayerChunkSystem");

}  // namespace ase::player
