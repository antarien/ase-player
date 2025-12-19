#include <ase/player/systems/player_input_system.hpp>
#include <ase/player/components/player_identity_component.hpp>
#include <ase/player/components/player_position_component.hpp>
#include <ase/player/components/player_config_component.hpp>
#include <ase/player/components/player_tags.hpp>
#include <ase/input/components/input_component.hpp>
#include <ase/input/systems/input_system.hpp>
#include <ase/camera/components/camera_component.hpp>
#include <ase/math/spherical.hpp>
#include <ase/ecs/schedule_registry.hpp>
#include <ase/log/log.hpp>

#include <cmath>

namespace ase::player {

// Use math constants from ase-math (SSOT)
using math::PI;
using math::TWO_PI;

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
        input::InputComponent
    >();

    for (auto [entity, identity, pos, input] : view.each()) {
        // Get camera for movement direction
        auto* cam = registry.try_get<camera::CameraComponent>(entity);

        // Movement direction comes from camera (or current player yaw if no camera)
        float movement_yaw = cam ? cam->movement_yaw : pos.yaw;

        // Check if player is actually moving
        const auto& mov = input.movement;
        bool is_moving = mov.forward != 0.0f || mov.strafe != 0.0f;

        // Only turn player body to face movement direction when:
        // 1. Player is actually moving (walking/running)
        // 2. NOT in orbit mode (orbit key not pressed)
        if (is_moving && cam && !cam->orbit_mode) {
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

REGISTER_SYSTEM(PlayerInputSystem)
    .in_schedule(ecs::Schedule::FixedUpdate)
    .with_priority(70)
    .run_after("TerrainChunkSystem");

}  // namespace ase::player
