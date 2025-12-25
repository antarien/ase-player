#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT
 * =============================================================================
 *
 * @file        player_st_mov_component.hpp
 * @brief       Movement settings singleton
 * @description Runtime values initialized from types.hpp constants
 *
 * -----------------------------------------------------------------------------
 * META
 * -----------------------------------------------------------------------------
 * @module      ase-player
 * @layer       3 (Module)
 * @category    state
 * @created     2025-12-25
 * @modified    2025-12-25
 * @version     1.0.0
 * @author      Jan Ohlmann (ADG/ASE/AOW)
 *
 * -----------------------------------------------------------------------------
 * ECS COMPONENT COMPLIANCE (15 Checkpoints)
 * -----------------------------------------------------------------------------
 * [x] DATA fields ONLY - No methods
 * [x] NO .cpp file - Header-only
 * [x] ONLY zero-initialization (= 0, = 0.0f, = false, = {})
 * [x] No magic numbers in defaults
 * [x] Entity references use InvalidEntityId (not 0)
 * [x] Single responsibility (one data category)
 * [x] No God-Component (unrelated fields)
 * [x] Large data uses pointer pattern (uint64_t ptr = 0)
 * [x] Tag structs end with Tag suffix
 * [x] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [x] Struct name derived from filename (snake_case to PascalCase)
 * [x] 1 File = 1 Component
 * [x] Folder in components/[category]/
 * [x] SHARED components listed in codegen.json components.shared
 * [x] Pointer components in codegen.json components.server_only
 *
 * =============================================================================
 */

#include <cstdint>

namespace ase::player {

struct PlayerStMovComponent {
    float walk_speed = 0.0f;
    float run_speed = 0.0f;
    float jump_impulse = 0.0f;
    float gravity = 0.0f;
    float ground_friction = 0.0f;
    float air_control = 0.0f;
    float ground_snap_dist = 0.0f;
    float turn_speed = 0.0f;
    float min_speed_threshold = 0.0f;
    float velocity_epsilon = 0.0f;
    float eye_height = 0.0f;
    float chunk_size = 0.0f;
};

}  // namespace ase::player
