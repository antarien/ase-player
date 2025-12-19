#include <ase/player/systems/player_physics_system.hpp>
#include <ase/player/components/player_position_component.hpp>
#include <ase/player/components/player_velocity_component.hpp>
#include <ase/player/components/player_physics_component.hpp>
#include <ase/player/components/player_config_component.hpp>
#include <ase/player/components/player_tags.hpp>
#include <ase/ecs/schedule_registry.hpp>
#include <ase/log/log.hpp>

#include <cmath>

namespace ase::player {

// =============================================================================
// LIFECYCLE
// =============================================================================

void PlayerPhysicsSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerPhysicsSystem] Started");
}

void PlayerPhysicsSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerPhysicsSystem] Stopped");
}

// =============================================================================
// TICK - Apply velocity and terrain collision
// =============================================================================

void PlayerPhysicsSystem::tick(ecs::Registry& registry, float dt) {
    // Get config from singleton (SSOT for constants)
    HeightQueryFn height_query;
    MovementConfig config;
    auto config_view = registry.view<PlayerConfigComponent>();
    for (auto [config_entity, cfg] : config_view.each()) {
        height_query = cfg.height_query;
        config = cfg.movement;
        break;
    }

    auto view = registry.view<
        PlayerPositionComponent,
        PlayerVelocityComponent,
        PlayerPhysicsComponent
    >();

    for (auto [entity, pos, vel, physics] : view.each()) {
        // Update position from velocity
        pos.x += vel.vx * dt;
        pos.y += vel.vy * dt;
        pos.z += vel.vz * dt;

        // Ground collision
        float ground_height = 0.0f;
        if (height_query) {
            ground_height = height_query(pos.x, pos.z);
        }

        if (pos.y <= ground_height + config.ground_snap_dist) {
            // On or slightly above ground - snap to ground
            pos.y = ground_height;
            vel.vy = 0.0f;
            physics.on_ground = true;
        } else {
            // Above ground
            physics.on_ground = false;
        }

        // Mark as dirty if moving
        float vel_len = std::sqrt(vel.vx * vel.vx + vel.vz * vel.vz);
        if (vel_len > config.velocity_epsilon) {
            registry.emplace_or_replace<PlayerDirtyTag>(entity);
        }
    }
}

// =============================================================================
// REGISTRATION
// =============================================================================

REGISTER_SYSTEM(PlayerPhysicsSystem)
    .in_schedule(ecs::Schedule::FixedUpdate)
    .with_priority(72)
    .run_after("PlayerMovementSystem");

}  // namespace ase::player
