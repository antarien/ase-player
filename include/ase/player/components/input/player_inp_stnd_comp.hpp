#pragma once

/**
 * ASE ECS COMPONENT (INPUT)
 *
 * @file        player_inp_stnd_comp.hpp
 * @brief       PlayerInpStndComponent - where ase-physics says the walking figure stands after its step
 * @description Bridge component: on a figure that walks as a body of ase-physics, the answer of that module
 *              (row HUB_PHYS_STAND on the figure entity) - the place after this tick's step relative to the
 *              frame the figure filed, the vertical speed and the placement count the place follows from.
 *              PlayerHubStndSyncSystem fills it in Reception; PlayerSimStndSystem takes the place over into
 *              PlayerStaPosComponent in Dynamics once the count matches the frame of the figure.
 *
 *              inp = input, stnd = stand
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    input
 * @parity      shared
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-14
 * @modified    2026-11-14
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * KUERZEL AUS DER TAXONOMIE: "stand": "stnd" (03_act_action), nachgeschlagen.
 *
 * SYN PATTERN (Hub I/O vs Calculation Separation)
 *
 * This component is the BRIDGE between:
 *   - Sync System (SERVER-ONLY): PlayerHubStndSyncSystem reads the Hub, writes this Component
 *   - Calc System (SHARED): PlayerSimStndSystem reads this Component, writes the place and the vertical speed
 *
 * HUB SOURCE (row HUB_PHYS_STAND on the figure entity, written by PhysicsHubStndPubSystem)
 *   HUB_PHYS_STAND_X → x      HUB_PHYS_STAND_Y → y      HUB_PHYS_STAND_Z → z
 *   HUB_PHYS_STAND_VEL_Y → vel_y      HUB_PHYS_STAND_SEQ → seq
 *   HUB_PHYS_STAND_VEL_X, HUB_PHYS_STAND_VEL_Z → PlayerInpStndVlcyComponent (the level speed of the same answer)
 *   mark HUB_PHYS_STAND_GRND → PlayerStndGrndTag (not a field: a tag-filtered view reads it)
 *
 * WHY shared: the device runs PlayerSimStndSystem on the same answer, so the place it draws is the place the
 * server settled; the field set is the wire both sides read.
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

#include <cstdint>

namespace ase::player {

/**
 * @brief Where ase-physics says the walking figure stands after its step
 */
struct PlayerInpStndComponent {
    float x = 0.0f;       // m - place along the axis of PLR_POS_X, relative to the frame origin the figure filed
    float y = 0.0f;       // m - height above the frame level the figure filed
    float z = 0.0f;       // m - place along the axis of PLR_POS_Y, relative to the frame origin the figure filed
    float vel_y = 0.0f;   // m/s - vertical speed after the step, upward positive; 0 on ground
    uint32_t seq = 0;     // the placement count of the frame this place follows from
};

}  // namespace ase::player
