#include <ase/player/systems/player_input_system.hpp>
#include <ase/player/components/player_identity_component.hpp>
#include <ase/player/components/player_position_component.hpp>
#include <ase/player/components/player_config_component.hpp>
#include <ase/player/components/player_tags.hpp>
#include <ase/input/components/input_movement_component.hpp>
#include <ase/input/components/input_camera_component.hpp>
#include <ase/input/components/input_meta_component.hpp>
#include <ase/camera/components/camera_orientation_component.hpp>
#include <ase/camera/components/camera_input_component.hpp>
// ase-network (ECS communication via Components)
#include <ase/network/components/player/state/network_plr_state_input_component.hpp>
#include <ase/math/spherical.hpp>
#include <ase/log/log.hpp>

#include <cmath>
#include <vector>

namespace ase::player {

// Use math constants from ase-math (SSOT)
using math::PI;
using math::TWO_PI;

namespace {

// Find player entity by player_id
ecs::Entity find_player_by_id(ecs::Registry& registry, const std::string& player_id) {
    auto view = registry.view<PlayerIdentityComponent>();
    for (auto [entity, identity] : view.each()) {
        if (identity.player_id == player_id) {
            return entity;
        }
    }
    return ecs::NullEntity;
}

// Process network input state and apply to player entities
// ECS Pattern: Network System writes NetworkPlrStateInputComponent → this reads it
void process_network_input(ecs::Registry& registry) {
    auto view = registry.view<network::NetworkPlrStateInputComponent>();
    std::vector<ecs::Entity> to_destroy;

    for (auto [input_entity, net_input] : view.each()) {
        std::string player_id(net_input.player_id.data());
        auto player_entity = find_player_by_id(registry, player_id);

        if (player_entity == ecs::NullEntity) {
            // Player not found, destroy the request entity
            to_destroy.push_back(input_entity);
            continue;
        }

        // Apply movement input to player
        auto* movement = registry.try_get<input::InputMovementComponent>(player_entity);
        if (movement) {
            movement->forward = net_input.forward;
            movement->strafe = net_input.strafe;
            movement->sprint = net_input.sprint;
            movement->jump = net_input.jump;
        }

        // Apply camera input to player
        auto* inp_cam = registry.try_get<input::InputCameraComponent>(player_entity);
        if (inp_cam) {
            inp_cam->yaw_delta = net_input.camera_yaw_delta;
            inp_cam->pitch_delta = net_input.camera_pitch_delta;
            inp_cam->zoom_delta = net_input.camera_zoom_delta;
            inp_cam->orbit_mode = net_input.camera_orbit_only;
            inp_cam->mode_toggle = net_input.camera_mode_toggle;
        }

        // Update camera orbit mode
        auto* cam_input = registry.try_get<camera::CameraInputComponent>(player_entity);
        if (cam_input) {
            cam_input->orbit_mode = net_input.camera_orbit_only;
        }

        // Update input meta
        auto* inp_meta = registry.try_get<input::InputMetaComponent>(player_entity);
        if (inp_meta) {
            inp_meta->camera_changed = (net_input.camera_yaw_delta != 0.0f ||
                                        net_input.camera_pitch_delta != 0.0f ||
                                        net_input.camera_zoom_delta != 0.0f ||
                                        net_input.camera_mode_toggle);
            inp_meta->movement_changed = (net_input.forward != 0.0f ||
                                          net_input.strafe != 0.0f);
            inp_meta->last_update = std::chrono::steady_clock::now();
        }

        // Mark request entity for destruction (processed)
        to_destroy.push_back(input_entity);
    }

    // Safe deletion: after iteration
    for (auto entity : to_destroy) {
        registry.destroy(entity);
    }
}

}  // anonymous namespace

// =============================================================================
// LIFECYCLE
// =============================================================================

void PlayerInputSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerInputSystem] Started");
}

void PlayerInputSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerInputSystem] Stopped");
}

// =============================================================================
// TICK - Process player input, update yaw rotation
// =============================================================================

void PlayerInputSystem::tick(ecs::Registry& registry, float dt) {
    // First: Process network input state → apply to player entities
    // ECS Pattern: NetworkDrainSystem writes NetworkPlrStateInputComponent,
    //              this System reads and applies to player input components
    process_network_input(registry);

    // Get config from singleton (SSOT for constants)
    MovementConfig config;
    auto config_view = registry.view<PlayerConfigComponent>();
    for (auto [config_entity, cfg] : config_view.each()) {
        config = cfg.movement;
        break;
    }

    auto view = registry.view<
        PlayerIdentityComponent,
        PlayerPositionComponent,
        input::InputMovementComponent
    >();

    for (auto [entity, identity, pos, movement] : view.each()) {
        // Get camera components for movement direction
        auto* cam_orient = registry.try_get<camera::CameraOrientationComponent>(entity);
        auto* cam_input = registry.try_get<camera::CameraInputComponent>(entity);

        // Movement direction comes from camera (or current player yaw if no camera)
        float movement_yaw = cam_orient ? cam_orient->movement_yaw : pos.yaw;

        // Check if player is actually moving
        bool is_moving = movement.forward != 0.0f || movement.strafe != 0.0f;

        // Check orbit mode (from camera input component)
        bool orbit_mode = cam_input ? cam_input->orbit_mode : false;

        // Only turn player body to face movement direction when:
        // 1. Player is actually moving (walking/running)
        // 2. NOT in orbit mode (orbit key not pressed)
        if (is_moving && cam_orient && !orbit_mode) {
            // Calculate shortest angle difference (handle wrap-around)
            float delta = movement_yaw - pos.yaw;
            while (delta > PI) delta -= TWO_PI;
            while (delta < -PI) delta += TWO_PI;

            // Smooth rotation interpolation
            float max_turn = config.turn_speed * dt;
            if (std::abs(delta) < max_turn) {
                pos.yaw = movement_yaw;
            } else {
                pos.yaw += (delta > 0 ? max_turn : -max_turn);
            }

            // Wrap yaw to [0, 2π]
            while (pos.yaw < 0.0f) pos.yaw += TWO_PI;
            while (pos.yaw >= TWO_PI) pos.yaw -= TWO_PI;
        }

        // Update last input timestamp
        identity.last_input = std::chrono::steady_clock::now();
    }
}

// =============================================================================
// REGISTRATION
// =============================================================================


}  // namespace ase::player
