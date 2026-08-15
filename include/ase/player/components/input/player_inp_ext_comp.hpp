#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT (INPUT)
 * =============================================================================
 *
 * @file        player_inp_ext_comp.hpp
 * @brief       External movement input data from Hub for player systems
 * @description Bridge component: Sync System writes Hub values here,
 *              Calc Systems read from here (SYN Pattern).
 *              Renamed from player_inp_ext_component.hpp 2026-08-15 (legacy
 *              suffix `_component`) and reduced from seven fields to four: the
 *              component exceeded the five-field limit. The camera values moved
 *              to PlayerInpCamComponent, the terrain height to
 *              PlayerInpTrnComponent. The cut follows the CONSUMPTION - these
 *              four axes share their readers, the other three did not.
 *
 * -----------------------------------------------------------------------------
 * META
 * -----------------------------------------------------------------------------
 * @module      ase-player
 * @layer       3 (Module)
 * @category    input
 * @created     2026-01-22
 * @modified    2026-08-15
 * @version     2.0.0
 *
 * -----------------------------------------------------------------------------
 * SYN PATTERN (Hub I/O vs Calculation Separation)
 * -----------------------------------------------------------------------------
 * This component is the BRIDGE between:
 *   - Sync System (SERVER-ONLY): Reads Hub, writes this Component
 *   - Calc System (SHARED): Reads this Component, writes State Components
 *
 * Hub Keys stored:
 *   PLR_INP_FWD    → inp_fwd    (Forward input -1 to 1)
 *   PLR_INP_STR    → inp_str    (Strafe input -1 to 1)
 *   PLR_INP_SPRINT → inp_sprint (Sprint flag 0 or 1)
 *   PLR_INP_JUMP   → inp_jump   (Jump flag 0 or 1)
 *
 * -----------------------------------------------------------------------------
 * ECS COMPONENT COMPLIANCE
 * -----------------------------------------------------------------------------
 * [ ] DATA fields ONLY - No methods
 * [ ] NO .cpp file - Header-only
 * [ ] ONLY zero-initialization (= 0, = 0.0f, = false, = {})
 * [ ] No magic numbers in defaults (use types.hpp constants)
 * [ ] Entity references initialized to = 0 (systems set values)
 * [ ] Single responsibility (one data category)
 * [ ] No God-Component (unrelated fields)
 * [ ] Large data in registry.ctx()? (component has only lookup ID!)
 * [ ] Tag structs end with Tag suffix - N/A (not a tag)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name derived from filename (snake_case to PascalCase)
 * [ ] 1 File = 1 Component
 * [ ] File in correct category subfolder
 * [ ] SHARED components listed in codegen.json components.shared
 * [ ] Pointer components in codegen.json components.server_only
 * [ ] Strings < 64 bytes use char[N] fixed arrays
 * [ ] Strings 64-256 bytes use appropriately sized char[N]
 * [ ] Strings > 256 bytes use registry.ctx() mit Lookup-ID?
 * [ ] NO Entity-per-Character (strings are single attributes, not N-Items!)
 * [ ] Lookup-only strings use uint32_t hash (entt::hashed_string)
 * [ ] NO std::shared_ptr in components (use Flyweight Pattern via ctx!)
 * [ ] NO void* in components (use Flyweight Pattern via ctx!)
 * [ ] NO uint64_t as pointer concept (use uint32_t ID + ResourceManager via ctx!)
 * [ ] External library objects (shared_ptr, handles) in ResourceManager via ctx()
 * [ ] Component stores ONLY primitive ID (uint32_t) referencing external resource
 *
 * =============================================================================
 */

#include <cstdint>

namespace ase::player {

/**
 * @brief Movement input axes mirrored from the Hub for this player entity.
 *
 * Written once per tick by PlayerSyncInpSystem (Integration) from the owner-keyed
 * hub values PLR_INP_FWD/STR/SPRINT/JUMP; read by PlayerCtrlMovSystem,
 * PlayerCtrlInpSystem and PlayerStaStsSystem. The two flags ride as 0.0f/1.0f
 * because the hub value space is float32 - the readers threshold them at 0.5f.
 */
struct PlayerInpExtComponent {
    float inp_fwd = 0.0f;     // Forward input (-1 to 1) from PLR_INP_FWD
    float inp_str = 0.0f;     // Strafe input (-1 to 1) from PLR_INP_STR
    float inp_sprint = 0.0f;  // Sprint input (0 or 1) from PLR_INP_SPRINT
    float inp_jump = 0.0f;    // Jump input (0 or 1) from PLR_INP_JUMP
};

}  // namespace ase::player
