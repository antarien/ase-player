#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT (INPUT)
 * =============================================================================
 *
 * @file        player_inp_trn_comp.hpp
 * @brief       External terrain height from Hub for player systems
 * @description Bridge component: Sync System writes Hub values here,
 *              Calc Systems read from here (SYN Pattern).
 *              Split out of PlayerInpExtComponent 2026-08-15: that component
 *              carried seven fields and exceeded the five-field limit. The cut
 *              follows the CONSUMPTION - this field has exactly one reader
 *              (PlayerSimPhysSystem, ground height) and shares no consumer with
 *              the input axes or the camera fields.
 *              The category is `input` because the ORIGIN is external, not
 *              because the value is a player input: `ext` means EXTERNAL, and
 *              TRN_HGT_AT_POS rides the same Hub bridge as the six PLR_* keys.
 *
 * -----------------------------------------------------------------------------
 * META
 * -----------------------------------------------------------------------------
 * @module      ase-player
 * @layer       3 (Module)
 * @category    input
 * @created     2026-08-15
 * @modified    2026-08-15
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
 *   TRN_HGT_AT_POS → trn_hgt    (Terrain height at position)
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
 * @brief Terrain height under this player entity, mirrored from the Hub.
 *
 * Written once per tick by PlayerSyncInpSystem (Integration) from the owner-keyed
 * hub value TRN_HGT_AT_POS, which ase-terrain publishes in Ingestion (11) - strictly
 * before Integration (12). Read by PlayerSimPhysSystem as the ground height for the
 * grounded check. The value crosses the module boundary over the hub star, never
 * through a shared row.
 */
struct PlayerInpTrnComponent {
    float trn_hgt = 0.0f;     // Terrain height from TRN_HGT_AT_POS
};

}  // namespace ase::player
