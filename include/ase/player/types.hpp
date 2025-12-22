#pragma once

/**
 * ASE Player Types - SSOT for constants
 *
 * RULES:
 * - NO enum class! Only constexpr uint8_t for enumeration values
 * - NO structs! Structs belong in Components
 * - ONLY: constexpr, using (simple type aliases like uint32_t)
 */

#include <cstdint>

namespace ase::player {

// =============================================================================
// PLAYER STATE (constexpr, NOT enum class!)
// =============================================================================

constexpr uint8_t PLAYER_STATE_IDLE = 0;
constexpr uint8_t PLAYER_STATE_WALKING = 1;
constexpr uint8_t PLAYER_STATE_RUNNING = 2;
constexpr uint8_t PLAYER_STATE_JUMPING = 3;
constexpr uint8_t PLAYER_STATE_FALLING = 4;
constexpr uint8_t PLAYER_STATE_SWIMMING = 5;
constexpr uint8_t PLAYER_STATE_DEAD = 6;

// =============================================================================
// LOG DEFAULTS
// =============================================================================

constexpr float LOG_DEFAULT_INTERVAL = 5.0f;

// =============================================================================
// MOVEMENT DEFAULTS (used to initialize PlayerStMovComponent)
// =============================================================================

// Movement speeds
constexpr float MOVEMENT_DEFAULT_WALK_SPEED = 4.0f;
constexpr float MOVEMENT_DEFAULT_RUN_SPEED = 8.0f;
constexpr float MOVEMENT_DEFAULT_JUMP_IMPULSE = 5.0f;

// Physics
constexpr float MOVEMENT_DEFAULT_GRAVITY = 9.81f;
constexpr float MOVEMENT_DEFAULT_GROUND_FRICTION = 10.0f;
constexpr float MOVEMENT_DEFAULT_AIR_CONTROL = 0.3f;
constexpr float MOVEMENT_DEFAULT_GROUND_SNAP_DIST = 0.1f;

// Rotation
constexpr float MOVEMENT_DEFAULT_TURN_SPEED = 10.0f;

// Thresholds
constexpr float MOVEMENT_DEFAULT_MIN_SPEED_THRESHOLD = 0.1f;
constexpr float MOVEMENT_DEFAULT_VELOCITY_EPSILON = 0.01f;

// Camera/View
constexpr float MOVEMENT_DEFAULT_EYE_HEIGHT = 1.0f;

// Chunk (from terrain, but default here for spawn)
constexpr float MOVEMENT_DEFAULT_CHUNK_SIZE = 32.0f;

}  // namespace ase::player
