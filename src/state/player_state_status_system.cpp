#include <ase/player/systems/state/player_state_status_system.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_phys_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>
#include <ase/player/types.hpp>
#include <ase/input/input.hpp>

#include <cmath>

namespace ase::player {

void PlayerStateStatusSystem::on_start(ecs::Registry& /*registry*/) {
}

void PlayerStateStatusSystem::on_stop(ecs::Registry& /*registry*/) {
}

void PlayerStateStatusSystem::tick(ecs::Registry& registry, float /*dt*/) {
    float min_speed = MOVEMENT_DEFAULT_MIN_SPEED_THRESHOLD;
    auto mov_view = registry.view<PlayerStMovComponent>();
    for (auto [e, mov] : mov_view.each()) {
        min_speed = mov.min_speed_threshold;
        break;
    }

    auto view = registry.view<
        PlayerStVelComponent,
        PlayerStPhysComponent,
        PlayerStStsComponent,
        input::InputStateMoveComponent
    >();

    for (auto [entity, vel, physics, state, movement] : view.each()) {
        float speed_xz = std::sqrt(vel.vx * vel.vx + vel.vz * vel.vz);

        if (physics.on_ground) {
            if (speed_xz > min_speed) {
                state.state = movement.sprint ? PLAYER_STATE_RUNNING : PLAYER_STATE_WALKING;
            } else {
                state.state = PLAYER_STATE_IDLE;
            }
        } else {
            if (vel.vy > 0.0f) {
                state.state = PLAYER_STATE_JUMPING;
            } else {
                state.state = PLAYER_STATE_FALLING;
            }
        }
    }
}

}  // namespace ase::player
