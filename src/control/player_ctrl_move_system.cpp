#include <ase/player/systems/control/player_ctrl_move_system.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_phys_component.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>
#include <ase/player/types.hpp>
#include <ase/input/input.hpp>
#include <ase/camera/camera.hpp>

#include <cmath>

namespace ase::player {

void PlayerCtrlMoveSystem::on_start(ecs::Registry& /*registry*/) {
}

void PlayerCtrlMoveSystem::on_stop(ecs::Registry& /*registry*/) {
}

void PlayerCtrlMoveSystem::tick(ecs::Registry& registry, float dt) {
    PlayerStMovComponent mov;
    mov.walk_speed = MOVEMENT_DEFAULT_WALK_SPEED;
    mov.run_speed = MOVEMENT_DEFAULT_RUN_SPEED;
    mov.jump_impulse = MOVEMENT_DEFAULT_JUMP_IMPULSE;
    mov.gravity = MOVEMENT_DEFAULT_GRAVITY;
    mov.ground_friction = MOVEMENT_DEFAULT_GROUND_FRICTION;
    mov.air_control = MOVEMENT_DEFAULT_AIR_CONTROL;

    auto mov_view = registry.view<PlayerStMovComponent>();
    for (auto [e, m] : mov_view.each()) {
        mov = m;
        break;
    }

    auto view = registry.view<
        PlayerStPosComponent,
        PlayerStVelComponent,
        PlayerStPhysComponent,
        input::InputStateMoveComponent
    >();

    for (auto [entity, pos, vel, physics, movement] : view.each()) {
        auto* cam_orient = registry.try_get<camera::CameraStOrtComponent>(entity);
        float movement_yaw = cam_orient ? cam_orient->movement_yaw : pos.yaw;

        float sin_yaw = std::sin(movement_yaw);
        float cos_yaw = std::cos(movement_yaw);

        float move_x = -movement.forward * sin_yaw + movement.strafe * cos_yaw;
        float move_z = -movement.forward * cos_yaw - movement.strafe * sin_yaw;

        float move_len = std::sqrt(move_x * move_x + move_z * move_z);
        if (move_len > 1.0f) {
            move_x /= move_len;
            move_z /= move_len;
        }

        float speed = movement.sprint ? mov.run_speed : mov.walk_speed;
        float control = physics.on_ground ? 1.0f : mov.air_control;

        float target_vx = move_x * speed;
        float target_vz = move_z * speed;

        if (physics.on_ground) {
            float accel = mov.ground_friction * dt;
            accel = std::min(accel, 1.0f);
            vel.vx += (target_vx - vel.vx) * accel;
            vel.vz += (target_vz - vel.vz) * accel;
        } else {
            vel.vx += (target_vx - vel.vx) * control * dt;
            vel.vz += (target_vz - vel.vz) * control * dt;
        }

        if (movement.jump && physics.on_ground) {
            vel.vy = mov.jump_impulse;
            physics.on_ground = false;
        }

        if (!physics.on_ground && physics.gravity_enabled) {
            vel.vy -= mov.gravity * dt;
        }
    }
}

}  // namespace ase::player
