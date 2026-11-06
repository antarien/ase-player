#pragma once

/**
 * ASE ECS COMPONENT (INPUT)
 *
 * @file        player_inp_cmb_comp.hpp
 * @brief       The movement restriction the combat star lays on a player figure
 * @description Bridge component: PlayerCmbSynSystem reads slow, frost and the two stuns of the
 *              combat star into this row in every Ingestion pass, and PlayerCtrlMovSystem scales the
 *              target speed of the figure from here instead of reading the hub (SYN Pattern).
 *
 * cmb = combat
 *
 * META
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    input
 * @parity      server_only
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-06
 * @modified    2026-11-06
 * @version     1.0.0
 *
 * SYN PATTERN (Hub I/O vs Calculation Separation)
 *
 * This component is the BRIDGE between:
 *   - Sync System (SERVER-ONLY): PlayerCmbSynSystem reads the Hub, writes this Component
 *   - Calc System: PlayerCtrlMovSystem reads this Component, scales the target speed
 *
 * Hub Keys read into it:
 *   CMB_SLW_INTENSITY        → slow_intensity  (while CMB_SLW_ACTIVE is set, fraction 0-1)
 *   CMB_FRZ_STA_SLOW_FACTOR  → freeze_slow     (movement reduction of frost, fraction 0-1)
 *   CMB_LANDING_STUN         → landing_stun    (stun after a landing, positive while it runs)
 *   CMB_PATZER_STUN          → patzer_stun     (recovery time left after a blunder)
 *
 * ZERO IS "NOT RESTRICTED". A key the star does not carry for the figure writes 0, so a figure
 * outside combat moves at full speed.
 *
 * ECS COMPONENT COMPLIANCE
 *
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
 * [ ] Strings > 256 bytes use registry.ctx() with lookup ID?
 * [ ] NO Entity-per-Character (strings are single attributes, not N-Items!)
 * [ ] Lookup-only strings use uint32_t hash (entt::hashed_string)
 * [ ] NO std::shared_ptr in components (use Flyweight Pattern via ctx!)
 * [ ] NO void* in components (use Flyweight Pattern via ctx!)
 * [ ] NO uint64_t as pointer concept (use uint32_t ID + ResourceManager via ctx!)
 * [ ] External library objects (shared_ptr, handles) in ResourceManager via ctx()
 * [ ] Component stores ONLY primitive ID (uint32_t) referencing external resource
 */

namespace ase::player {

/**
 * @brief Slow, frost and stun of a player figure, as the combat star lays them.
 *
 * Written every Ingestion pass by PlayerCmbSynSystem on each player figure; read by
 * PlayerCtrlMovSystem in STEP 5 of the mover.
 */
struct PlayerInpCmbComponent {
    float slow_intensity = 0.0f;  // Slow fraction of the target speed (CMB_SLW_INTENSITY)
    float freeze_slow = 0.0f;     // Frost fraction of the target speed (CMB_FRZ_STA_SLOW_FACTOR)
    float landing_stun = 0.0f;    // Landing stun, positive while it runs (CMB_LANDING_STUN)
    float patzer_stun = 0.0f;     // Recovery time left after a blunder (CMB_PATZER_STUN)
};

}  // namespace ase::player
