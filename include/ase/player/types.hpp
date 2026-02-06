#pragma once

/**
 * ASE MODULE TYPES (SSOT)
 *
 * @file        types.hpp
 * @brief       Single Source of Truth for ase-player constants and types
 * @description All compile-time constants, type aliases, and enumeration values.
 *              NO runtime values - those belong in Components!
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @created     2025-12-01
 * @modified    2026-01-22
 * @version     2.0.0
 *
 * ECS TYPES COMPLIANCE
 *
 * [ ] All constants defined (no magic numbers in code)
 * [ ] Every constant has inline comment (English, explains purpose)
 * [ ] NO enum class (only constexpr uint8_t for enumeration values)
 * [ ] Type aliases defined
 * [ ] InvalidEntityId = UINT32_MAX defined (if needed)
 * [ ] Abbreviations documented
 * [ ] NO structs (structs belong in Components)
 */

#include <cstdint>
#include <ase/types/types.hpp>

namespace ase::player {

/**
 * TYPE ALIASES
 * Custom type definitions for this module.
 */
using PlayerId = uint32_t;  // ID type for player entities
using TypId = uint16_t;     // Network serialization type identifier

/**
 * INVALID MARKERS
 * Sentinel values for uninitialized/invalid data.
 */
constexpr uint32_t InvalidPlayerId = 0;           // Invalid player ID sentinel
using ase::types::InvalidEntityId;                // SSOT: ase-types (Layer 0)

/**
 * SERIAL TYPE IDs (Layer 3: 1-999)
 * Network serialization type identifiers.
 */
constexpr TypId SERIAL_TYP_PLR_SPN = 1;  // Player spawn (id + pos)
constexpr TypId SERIAL_TYP_PLR_STA = 2;  // Player state (id + pos + vel + sts)

/**
 * BROADCAST CHANNELS
 * Network channel identifiers for player data.
 */
constexpr const char* CHANNEL_PLR_SPN = "player_spawn";  // Spawn broadcast channel
constexpr const char* CHANNEL_PLR_STA = "player_state";  // State broadcast channel
constexpr uint32_t CHANNEL_PLR_SPN_HASH = 0x706C7273;    // FNV-1a hash of "player_spawn"
constexpr uint32_t CHANNEL_PLR_STA_HASH = 0x706C7274;    // FNV-1a hash of "player_state"

/**
 * ENUMERATION VALUES - PLAYER STATE (NO enum class!)
 * Use with Tag-filtered Views, NOT switch/case!
 */
constexpr uint8_t PLAYER_STATE_IDLE = 0;      // Player is idle
constexpr uint8_t PLAYER_STATE_WALKING = 1;   // Player is walking
constexpr uint8_t PLAYER_STATE_RUNNING = 2;   // Player is running
constexpr uint8_t PLAYER_STATE_JUMPING = 3;   // Player is jumping
constexpr uint8_t PLAYER_STATE_FALLING = 4;   // Player is falling
constexpr uint8_t PLAYER_STATE_SWIMMING = 5;  // Player is swimming
constexpr uint8_t PLAYER_STATE_DEAD = 6;      // Player is dead

/**
 * DEFAULT VALUES - MOVEMENT SPEEDS
 * Fallback values for PlayerStMovComponent initialization.
 */
constexpr float MOVEMENT_DEFAULT_WALK_SPEED = 4.0f;     // Walk speed (m/s)
constexpr float MOVEMENT_DEFAULT_RUN_SPEED = 8.0f;      // Run speed (m/s)
constexpr float MOVEMENT_DEFAULT_JUMP_IMPULSE = 5.0f;   // Jump impulse (m/s)

/**
 * DEFAULT VALUES - PHYSICS
 * Physics simulation parameters.
 * NOTE: Gravity constant imported from ase-math PHYSICS_GRAVITY_EARTH_MS2
 */
constexpr float MOVEMENT_DEFAULT_GRAVITY = 9.8f;           // Gravity approximation (m/s^2)
constexpr float MOVEMENT_DEFAULT_GROUND_FRICTION = 1e1f;   // Ground friction coefficient
constexpr float MOVEMENT_DEFAULT_AIR_CONTROL = 0.3f;       // Air control factor (0-1)
constexpr float MOVEMENT_DEFAULT_GROUND_SNAP_DIST = 0.1f;  // Ground snap distance (m)

/**
 * DEFAULT VALUES - ROTATION
 * Rotation parameters.
 */
constexpr float MOVEMENT_DEFAULT_TURN_SPEED = 1e1f;  // Turn speed (rad/s)

/**
 * DEFAULT VALUES - THRESHOLDS
 * Movement threshold values.
 */
constexpr float MOVEMENT_DEFAULT_MIN_SPEED_THRESHOLD = 0.1f;  // Min speed threshold (m/s)
constexpr float MOVEMENT_DEFAULT_VELOCITY_EPSILON = 0.01f;    // Velocity epsilon (m/s)

/**
 * DEFAULT VALUES - CAMERA/VIEW
 * Camera and view parameters.
 */
constexpr float MOVEMENT_DEFAULT_EYE_HEIGHT = 1.0f;  // Eye height (m)

/**
 * DEFAULT VALUES - CHUNK
 * Chunk size for spatial calculations.
 */
constexpr float MOVEMENT_DEFAULT_CHUNK_SIZE = 32.0f;  // Chunk size (m)

/**
 * MIGRATED TO hub_constants.json
 * The following constants are now in hub/data/hub_constants.json (SSOT):
 *   - LOG_CONST_DEFAULT_INTERVAL = 5.0 (was LOG_DEFAULT_INTERVAL)
 */
constexpr float PLR_LOG_INTERVAL_FALLBACK = 5.0f;  // Log interval fallback (seconds) when Hub unavailable

/**
 * ABBREVIATIONS (Documentation)
 * Used in filenames ONLY, between prefix and suffix.
 * Folder names are SPELLED OUT (state/, not sta/).
 *
 * Standard abbreviations used in this module (3-4 chars):
 *
 * │ Full Word │ Abbr │ Example                          │
 * │───────────│──────│──────────────────────────────────│
 * │ state     │ st   │ player_st_mov_component.hpp      │
 * │ tag       │ tag  │ player_tag_ded_component.hpp     │
 * │ movement  │ mov  │ player_st_mov_component.hpp      │
 * │ spawn     │ spn  │ player_spn_system.hpp            │
 * │ position  │ pos  │ player_st_pos_component.hpp      │
 * │ velocity  │ vel  │ player_st_vel_component.hpp      │
 * │ status    │ sts  │ player_st_sts_component.hpp      │
 * │ dead      │ ded  │ player_tag_ded_component.hpp     │
 *
 * NOTE: Folder names are SPELLED OUT (state/, not sta/)
 * NOTE: Abbreviations appear ONLY in filenames between prefix and suffix
 */

}  // namespace ase::player
