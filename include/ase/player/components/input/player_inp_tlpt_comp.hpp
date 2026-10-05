#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT (INPUT)
 * =============================================================================
 *
 * @file        player_inp_tlpt_comp.hpp
 * @brief       The teleport jump a backend-driven cheater makes per tick, mirrored from the Hub
 * @description Bridge component: PlayerSyncTlptSystem writes the backend lever here, the calc
 *              system PlayerSimTlptSystem reads it (SYN Pattern). Phase 13, register D0703: the
 *              teleport lever moves the POSITION of a real ase-player entity each tick without any
 *              velocity, so the teleport authority of PlayerAccMovSystem (PLR_AC_TELEPORT_STEP) is
 *              exercised on its own — the speed lever PLR_CHEAT_SPEED proves only the speed
 *              authority. Its own component and not a field beside the speed state: one lever, one
 *              consumer, the same cut as PlayerInpTrnComponent (the cut follows the consumption).
 *              The category is `input` because the ORIGIN is external (the operator's lever), not
 *              because the value is a player input.
 *
 *              tlpt = teleport (taxonomy catalogue, act/nav)
 *
 * -----------------------------------------------------------------------------
 * META
 * -----------------------------------------------------------------------------
 * @module      ase-player
 * @layer       3 (Module)
 * @category    input
 * @parity      shared
 * @created     2026-10-03
 * @modified    2026-10-03
 * @version     1.0.0
 *
 * -----------------------------------------------------------------------------
 * SYN PATTERN (Hub I/O vs Calculation Separation)
 * -----------------------------------------------------------------------------
 * This component is the BRIDGE between:
 *   - Sync System (SERVER-ONLY): Reads Hub, writes this Component
 *   - Calc System (SHARED): Reads this Component, writes State Components
 *
 * Hub Keys stored:
 *   PLR_CHEAT_TELEPORT → jump_m    (per-tick position jump, owner = hash(player_id))
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
 * @brief The per-tick teleport jump of a backend-driven cheater, mirrored from the Hub.
 *
 * Written by PlayerSyncTlptSystem (Integration) from PLR_CHEAT_TELEPORT under owner =
 * hash(player_id); present only on a player the lever was ever set for. Read by
 * PlayerSimTlptSystem (Dynamics), which adds jump_m to the chunk-relative position before the
 * physics step. 0 = the lever was cleared, no jump.
 */
struct PlayerInpTlptComponent {
    float jump_m = 0.0f;  // Per-tick position jump (m); above PLR_AC_TELEPORT_STEP it is a teleport
};

}  // namespace ase::player
