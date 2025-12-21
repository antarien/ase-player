#include <ase/player/systems/state/player_state_status_system.hpp>
#include <ase/player/components/state/player_state_vel_component.hpp>
#include <ase/player/components/state/player_state_phys_component.hpp>
#include <ase/player/components/state/player_state_status_component.hpp>
#include <ase/player/components/state/player_state_cfg_component.hpp>
#include <ase/input/input.hpp>
#include <ase/log/log.hpp>

#include <cmath>

namespace ase::player {

void PlayerStateStatusSystem::on_start(ecs::Registry& /*registry*/) {
    // No resources to initialize - pure state processing system
}

void PlayerStateStatusSystem::on_stop(ecs::Registry& /*registry*/) {
    // No resources to cleanup - pure state processing system
}

void PlayerStateStatusSystem::tick(ecs::Registry& registry, float /*dt*/) {
    MovementConfig config;
    auto config_view = registry.view<PlayerStateCfgComponent>();
    for (auto [config_entity, cfg] : config_view.each()) {
        config = cfg.movement;
        break;
    }

    auto view = registry.view<
        PlayerStateVelComponent,
        PlayerStatePhysComponent,
        PlayerStateStatusComponent,
        input::InputStateMoveComponent
    >();

    for (auto [entity, vel, physics, state, movement] : view.each()) {
        float speed_xz = std::sqrt(vel.vx * vel.vx + vel.vz * vel.vz);

        if (physics.on_ground) {
            if (speed_xz > config.min_speed_threshold) {
                state.state = movement.sprint ? PlayerState::Running : PlayerState::Walking;
            } else {
                state.state = PlayerState::Idle;
            }
        } else {
            if (vel.vy > 0.0f) {
                state.state = PlayerState::Jumping;
            } else {
                state.state = PlayerState::Falling;
            }
        }
    }
}

}  // namespace ase::player
