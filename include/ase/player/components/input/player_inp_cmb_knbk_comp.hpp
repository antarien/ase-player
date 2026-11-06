#pragma once

/**
 * ASE ECS COMPONENT (INPUT)
 *
 * @file        player_inp_cmb_knbk_comp.hpp
 * @brief       The knockback displacement the combat star lays on a player figure in this frame
 * @description Bridge component: PlayerCmbSynSystem reads the three knockback displacements of the
 *              combat star into this row while a knockback runs; PlayerSimKnbkSystem moves the
 *              position by them and PlayerAccMovSystem subtracts them from the teleport delta
 *              (SYN Pattern).
 *
 * cmb = combat, knbk = knockback
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
 *   - Calc System: PlayerSimKnbkSystem reads this Component, moves the position
 *
 * Hub Keys read into it:
 *   CMB_KBK_DISP_X → disp_x  (metres in this frame, first ground axis)
 *   CMB_KBK_DISP_Y → disp_y  (metres in this frame, second ground axis)
 *   CMB_KBK_DISP_Z → disp_z  (metres in this frame, height)
 *
 * THE AXES ARE THOSE OF THE COMBAT STAR. CmbStatKbkDispComponent keeps X and Y on the ground and Z
 * as the height; the player figure keeps local_x and local_z on the ground and y as the height.
 * PlayerSimKnbkSystem maps disp_x onto local_x, disp_y onto local_z and disp_z onto y.
 *
 * THE PRESENCE IS THE KNOCKBACK. The row stands only while CMB_KBK_ACTIVE is set and all three
 * displacements are found; without a knockback it is removed.
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
 * @brief Knockback displacement of a player figure in this frame, in metres.
 *
 * Laid every Ingestion pass by PlayerCmbSynSystem while a knockback runs, removed otherwise.
 */
struct PlayerInpCmbKnbkComponent {
    float disp_x = 0.0f;  // Ground displacement along combat X in this frame, metres (CMB_KBK_DISP_X)
    float disp_y = 0.0f;  // Ground displacement along combat Y in this frame, metres (CMB_KBK_DISP_Y)
    float disp_z = 0.0f;  // Height displacement in this frame, metres (CMB_KBK_DISP_Z)
};

}  // namespace ase::player
