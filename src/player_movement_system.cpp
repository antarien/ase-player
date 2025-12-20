#include <ase/player/systems/player_movement_system.hpp>
#include <ase/player/components/player_position_component.hpp>
#include <ase/player/components/player_velocity_component.hpp>
#include <ase/player/components/player_physics_component.hpp>
#include <ase/player/components/player_config_component.hpp>
#include <ase/input/components/input_movement_component.hpp>
#include <ase/camera/components/camera_orientation_component.hpp>
#include <ase/log/log.hpp>

#include <cmath>

namespace ase::player {

// =============================================================================
// LIFECYCLE
// =============================================================================

void PlayerMovementSystem::on_start(ecs::Registry& /*registry*/) {
}

void PlayerMovementSystem::on_stop(ecs::Registry& /*registry*/) {
}

// =============================================================================
// TICK - Calculate velocity from input
// =============================================================================

void PlayerMovementSystem::tick(ecs::Registry& registry, float dt) {
    // Get movement config from singleton
    MovementConfig config;
    auto config_view = registry.view<PlayerConfigComponent>();
    for (auto [config_entity, cfg] : config_view.each()) {
        config = cfg.movement;
        break;  // Only one config
    }

    auto view = registry.view<
        PlayerPositionComponent,
        PlayerVelocityComponent,
        PlayerPhysicsComponent,
        input::InputMovementComponent
    >();

    for (auto [entity, pos, vel, physics, movement] : view.each()) {
        // Get camera orientation for movement direction
        auto* cam_orient = registry.try_get<camera::CameraOrientationComponent>(entity);
        float movement_yaw = cam_orient ? cam_orient->movement_yaw : pos.yaw;

        // Calculate movement direction based on camera yaw
        // Note: In Three.js, -Z is forward
        float sin_yaw = std::sin(movement_yaw);
        float cos_yaw = std::cos(movement_yaw);

        float move_x = -movement.forward * sin_yaw + movement.strafe * cos_yaw;
        float move_z = -movement.forward * cos_yaw - movement.strafe * sin_yaw;

        // Normalize if moving diagonally
        float move_len = std::sqrt(move_x * move_x + move_z * move_z);
        if (move_len > 1.0f) {
            move_x /= move_len;
            move_z /= move_len;
        }

        // Determine speed
        float speed = movement.sprint ? config.run_speed : config.walk_speed;
        float control = physics.on_ground ? 1.0f : config.air_control;

        // Target velocity
        float target_vx = move_x * speed;
        float target_vz = move_z * speed;

        // Smooth acceleration
        if (physics.on_ground) {
            float accel = config.ground_friction * dt;
            accel = std::min(accel, 1.0f);
            vel.vx += (target_vx - vel.vx) * accel;
            vel.vz += (target_vz - vel.vz) * accel;
        } else {
            // Air control is limited
            vel.vx += (target_vx - vel.vx) * control * dt;
            vel.vz += (target_vz - vel.vz) * control * dt;
        }

        // Jump
        if (movement.jump && physics.on_ground) {
            vel.vy = config.jump_impulse;
            physics.on_ground = false;
        }

        // Apply gravity
        if (!physics.on_ground && physics.gravity_enabled) {
            vel.vy -= config.gravity * dt;
        }
    }
}

// =============================================================================
// REGISTRATION
// =============================================================================


}  // namespace ase::player
