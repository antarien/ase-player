#include <ase/player/systems/player_state_system.hpp>
#include <ase/player/components/player_velocity_component.hpp>
#include <ase/player/components/player_physics_component.hpp>
#include <ase/player/components/player_state_component.hpp>
#include <ase/player/components/player_config_component.hpp>
#include <ase/input/components/input_movement_component.hpp>
#include <ase/log/log.hpp>

#include <cmath>

namespace ase::player {

// =============================================================================
// LIFECYCLE
// =============================================================================

void PlayerStateSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerStateSystem] Started");
}

void PlayerStateSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerStateSystem] Stopped");
}

// =============================================================================
// TICK - Update player state machine
// =============================================================================

void PlayerStateSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // Get config from singleton (SSOT for constants)
    MovementConfig config;
    auto config_view = registry.view<PlayerConfigComponent>();
    for (auto [config_entity, cfg] : config_view.each()) {
        config = cfg.movement;
        break;
    }

    auto view = registry.view<
        PlayerVelocityComponent,
        PlayerPhysicsComponent,
        PlayerStateComponent,
        input::InputMovementComponent
    >();

    for (auto [entity, vel, physics, state, movement] : view.each()) {
        // Calculate horizontal speed
        float speed_xz = std::sqrt(vel.vx * vel.vx + vel.vz * vel.vz);

        if (physics.on_ground) {
            // On ground: Idle, Walking, or Running
            if (speed_xz > config.min_speed_threshold) {
                state.state = movement.sprint ? PlayerState::Running : PlayerState::Walking;
            } else {
                state.state = PlayerState::Idle;
            }
        } else {
            // In air: Jumping or Falling
            if (vel.vy > 0.0f) {
                state.state = PlayerState::Jumping;
            } else {
                state.state = PlayerState::Falling;
            }
        }

        // NOTE: Input flags are now cleared by InputMetaSystem (Single Responsibility!)
        // Do NOT call InputSystem::clear_flags() here - that was a layering violation
    }
}

// =============================================================================
// REGISTRATION
// =============================================================================


}  // namespace ase::player
