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
 * ANTI-CHEAT MOVEMENT AUTHORITY (Phase 13 / Task 13.10)
 * Engine-level deterministic bound on a player's realised horizontal speed. A legit player's
 * velocity is capped at run_speed by PlayerCtrlMovSystem, so a realised horizontal speed above
 * run_speed * margin cannot come from legitimate input — it is a speed-hack. The detector compares
 * the SQUARED horizontal speed (vx*vx + vz*vz) against the squared bound (no sqrt, frequency-
 * independent since velocity is m/s). A flagged player drives the contract trigger topic
 * "PLAYER_MOVEMENT_SUSPICIOUS" (hashed at the Hub call site), consumed by MovementValidator.
 */
constexpr float PLR_AC_SPEED_MARGIN = 1.5f;  // Tolerance factor over run_speed (slopes, knockback, latency)
constexpr float PLR_AC_MAX_HORIZONTAL_SPEED = MOVEMENT_DEFAULT_RUN_SPEED * PLR_AC_SPEED_MARGIN;  // Max plausible horizontal speed (m/s)
constexpr float PLR_AC_MAX_HORIZONTAL_SPEED_SQ = PLR_AC_MAX_HORIZONTAL_SPEED * PLR_AC_MAX_HORIZONTAL_SPEED;  // Squared bound vs vx*vx + vz*vz

/**
 * ANTI-CHEAT TELEPORT AUTHORITY (Phase 13 / Task 13.10)
 * A legit player moves at most run_speed*margin per second; at the Dynamics (30Hz) detector rate a single-
 * tick horizontal position delta is sub-metre. A per-tick delta exceeding PLR_AC_TELEPORT_STEP cannot be
 * legitimate motion — it is a teleport-hack. The detector compares the SQUARED per-tick position delta
 * (dx*dx + dz*dz) against the squared bound (no sqrt). The flagged player drives the SAME contract trigger
 * "PLAYER_MOVEMENT_SUSPICIOUS". PlayerStaAcpComponent holds the previous observed position per player.
 */
constexpr float PLR_AC_TELEPORT_STEP = PLR_AC_MAX_HORIZONTAL_SPEED;  // Max plausible single-tick horizontal position delta (m); larger = teleport
constexpr float PLR_AC_TELEPORT_STEP_SQ = PLR_AC_TELEPORT_STEP * PLR_AC_TELEPORT_STEP;  // Squared bound vs dx*dx + dz*dz

/**
 * ANTI-CHEAT ARCHETYPE AUTHORITIES (Phase 13 AP-3)
 * Engine-level deterministic bounds on the realised per-player activity RATES of the remaining
 * contract-topic families (ANTI_CHEAT_COMPONENT_CONTRACT.md): a realised rate above the bound cannot
 * come from legitimate play — human input, chat cadence and economy flow are all physically capped.
 * Each detector compares its realised rate component against ONE bound and drives ONE contract topic
 * (exact-hash literal, the `*` is PART of the topic name per the contract):
 *   PlayerStaActComponent.action_rate        > PLR_AC_MAX_ACTION_RATE       → "PLAYER_ACTION_*"        (BehaviorWatcher)
 *   PlayerStaCmbComponent.combat_rate        > PLR_AC_MAX_COMBAT_RATE       → "COMBAT_EVENT_*"         (BehaviorWatcher)
 *   PlayerStaComComponent.message_rate       > PLR_AC_MAX_MESSAGE_RATE      → "PLAYER_COMMUNICATION_*" (CoordinationDetector)
 *   PlayerStaEcoComponent.transaction_rate   > PLR_AC_MAX_TRANSACTION_RATE  → "ECONOMY_TRANSACTION_*"  (EconomyAuditor)
 *   PlayerStaInvComponent.modification_rate  > PLR_AC_MAX_INVENTORY_RATE    → "INVENTORY_MODIFICATION_*" (EconomyAuditor)
 * The realised rates are driven by the game simulation; the backend cheat levers (PLR_CHEAT_ACTIONS /
 * PLR_CHEAT_CHAT / PLR_CHEAT_DUPE, frame 76) induce them deterministically on the REAL player entity —
 * never a fabricated topic inject (same honesty rule as the speed lever).
 */
constexpr float PLR_AC_MAX_ACTION_RATE      = 20.0f;  // Max plausible player actions/sec (human input cap)
constexpr float PLR_AC_MAX_COMBAT_RATE      = 10.0f;  // Max plausible combat events/sec a player can cause
constexpr float PLR_AC_MAX_MESSAGE_RATE     = 5.0f;   // Max plausible chat messages/sec (human typing cap)
constexpr float PLR_AC_MAX_TRANSACTION_RATE = 8.0f;   // Max plausible economy transactions/sec
constexpr float PLR_AC_MAX_INVENTORY_RATE   = 15.0f;  // Max plausible inventory modifications/sec

/**
 * BACKEND-DRIVEN SPAWN WIRE (Phase 13 / Task 13.10)
 * The Replica forwards the ase-cli spawn frame VERBATIM to the World publisher; PlayerSpwnRcvSystem drains
 * transport::LANE_SPW and creates a PlayerReqSpawnComponent so PlayerLifeSpwnSystem spawns the real entity
 * (no human WebRTC client). Local mirror of ase-network BIN_MSG_PLAYER_SPAWN=77 (L3→L3 forbidden — the bytes
 * are restated here). Frame: [77][player_id:char[64]][x:f32][z:f32] = 73 bytes.
 */
constexpr uint8_t  PLR_BIN_MSG_PLAYER_SPAWN = 77;  // mirror of ase-network BIN_MSG_PLAYER_SPAWN (76 = PLAYER_CHEAT)
constexpr uint32_t PLR_SPW_FRAME_SZ         = 73u; // 1 type + 64 player_id + 4 x + 4 z
constexpr uint32_t PLR_SPW_PLAYER_ID_OFF    = 1u;  // player_id:char[64] offset (after the type byte)
constexpr uint32_t PLR_SPW_PLAYER_ID_LEN    = 64u; // player_id fixed field length (matches PlayerReqSpawnComponent.player_id[64])
constexpr uint32_t PLR_SPW_X_OFF            = 65u; // spawn x:f32 offset
constexpr uint32_t PLR_SPW_Z_OFF            = 69u; // spawn z:f32 offset
constexpr uint32_t PLR_SPW_RCV_BATCH_MAX    = 32u; // max spawn frames drained per tick (bounded)

/**
 * SESSION INDEX REGISTER (PlayerHubSessRegSystem)
 *
 * Hub v2.0 has no iteration API, so live players are addressed through registered index slots:
 * PLR_ACTIVE_COUNT (GLOBAL) says how many are live, the owner of slot i is the hash of
 * "PLR_ACTIVE_<i>", and PLR_OWNER read at that owner yields the player entity id. The slot bound
 * is a WIRE CONTRACT with the consumers - it must not exceed what they walk per pass
 * (ase-terrain TRN_OBS_SES_SLOT_MAX, terrain/types.hpp:208); a larger value here would publish
 * slots that no consumer ever reads. L3 modules may not include each other, so the bound is
 * restated rather than shared.
 */
constexpr uint32_t PLR_SESS_SLOT_MAX        = 256u; // max published PLR_ACTIVE_<i> index slots
constexpr uint32_t PLR_SESS_KEY_MAX         = 32u;  // char[N] capacity of a built "PLR_ACTIVE_<i>" key
constexpr uint32_t PLR_SESS_DIGIT_MAX       = 10u;  // decimal digits a uint32 slot index can carry

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
