#pragma once

/**
 * ASE Player Types
 *
 * Player-specific types. Uses ase-math for Vec3 and ase-input for input types.
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
 * Movement constants
 */
struct MovementConfig {
    float walk_speed = 4.0f;        // m/s
    float run_speed = 8.0f;         // m/s
    float jump_velocity = 5.0f;     // m/s
    float gravity = 9.81f;          // m/s^2
    float ground_friction = 10.0f;  // Deceleration factor
    float air_control = 0.3f;       // Air movement multiplier
};

}  // namespace ase::player
