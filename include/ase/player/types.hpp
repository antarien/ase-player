#pragma once

/**
 * ASE Player Types
 *
 * Player-specific types. Uses ase-math for Vec3 and ase-input for input types.
 *
 * SSOT: All player-related constants are defined here in MovementConfig.
 * Systems must NOT have file-level static/constexpr (per ARCH_ASE_ECS_CRASHKURS.md).
 */

#include <ase/math/vec3.hpp>
#include <ase/input/types.hpp>
#include <cstdint>

namespace ase::player {

// Use ase::math::Vec3 for all vector operations
using Vec3 = ase::math::Vec3;

// Use ase::input::MovementInput for player movement
using MovementInput = ase::input::MovementInput;

/**
 * Player state flags
 */
enum class PlayerState : uint8_t {
    Idle = 0,
    Walking = 1,
    Running = 2,
    Jumping = 3,
    Falling = 4,
    Swimming = 5,
    Dead = 6
};

/**
 * Movement constants - SSOT for all player systems
 *
 * All player-related magic numbers MUST be here.
 * Systems read from PlayerConfigComponent which contains this config.
 */
struct MovementConfig {
    // Movement speeds
    float walk_speed = 4.0f;         // m/s
    float run_speed = 8.0f;          // m/s
    float jump_impulse = 5.0f;       // m/s initial jump velocity

    // Physics
    float gravity = 9.81f;           // m/s^2
    float ground_friction = 10.0f;   // Deceleration factor
    float air_control = 0.3f;        // Air movement multiplier (0-1)
    float ground_snap_dist = 0.1f;   // Distance to snap to ground

    // Rotation
    float turn_speed = 10.0f;        // Radians per second (body rotation)

    // Thresholds
    float min_speed_threshold = 0.1f; // Min speed to count as "moving"
    float velocity_epsilon = 0.01f;   // Velocity below this is zero

    // Camera/View
    float eye_height = 1.0f;         // Height offset for camera target

    // Chunk (from terrain, but default here for spawn)
    float chunk_size = 32.0f;        // Meters per chunk
};

}  // namespace ase::player
