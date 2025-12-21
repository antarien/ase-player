#include <ase/player/systems/control/player_ctrl_move_system.hpp>
#include <ase/player/components/state/player_state_pos_component.hpp>
#include <ase/player/components/state/player_state_vel_component.hpp>
#include <ase/player/components/state/player_state_phys_component.hpp>
#include <ase/player/components/state/player_state_cfg_component.hpp>
#include <ase/input/input.hpp>
#include <ase/camera/camera.hpp>
#include <ase/log/log.hpp>

#include <cmath>

namespace ase::player {

void PlayerCtrlMoveSystem::on_start(ecs::Registry& /*registry*/) {
}

void PlayerCtrlMoveSystem::on_stop(ecs::Registry& /*registry*/) {
}

void PlayerCtrlMoveSystem::tick(ecs::Registry& registry, float dt) {
    MovementConfig config;
    auto config_view = registry.view<PlayerStateCfgComponent>();
    for (auto [config_entity, cfg] : config_view.each()) {
        config = cfg.movement;
        break;
    }

    auto view = registry.view<
        PlayerStatePosComponent,
        PlayerStateVelComponent,
        PlayerStatePhysComponent,
        input::InputStateMoveComponent
    >();

    for (auto [entity, pos, vel, physics, movement] : view.each()) {
        auto* cam_orient = registry.try_get<camera::CameraStateOrientComponent>(entity);
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

        float speed = movement.sprint ? config.run_speed : config.walk_speed;
        float control = physics.on_ground ? 1.0f : config.air_control;

        float target_vx = move_x * speed;
        float target_vz = move_z * speed;

        if (physics.on_ground) {
            float accel = config.ground_friction * dt;
            accel = std::min(accel, 1.0f);
            vel.vx += (target_vx - vel.vx) * accel;
            vel.vz += (target_vz - vel.vz) * accel;
        } else {
            vel.vx += (target_vx - vel.vx) * control * dt;
            vel.vz += (target_vz - vel.vz) * control * dt;
        }

        if (movement.jump && physics.on_ground) {
            vel.vy = config.jump_impulse;
            physics.on_ground = false;
        }

        if (!physics.on_ground && physics.gravity_enabled) {
            vel.vy -= config.gravity * dt;
        }
    }
}

}  // namespace ase::player
