#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_sta_plcm_comp.hpp
 * @brief       PlayerStaPlcmComponent - on which plate a walking figure was placed, and how often
 * @description The placement of a figure that walks as a body of ase-physics. owner names the area plate
 *              whose disc is the frame (PlayerStaFrmComponent), or InvalidEntityId for the frame of a chunk
 *              corner; ase-physics reads it as HUB_PHYS_WALK_OWNER to find the disc and the terrain under it.
 *              seq counts the placements: it rises each time the figure is put somewhere, ase-physics takes a
 *              new place and frame only when it moves, and answers with the count its place follows from
 *              (HUB_PHYS_STAND_SEQ). An answer to an older count is never taken over.
 *
 *              sta = state, plcm = placement
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state
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
 * KUERZEL AUS DER TAXONOMIE: "placement": "plcm" (03_act_action), nachgeschlagen.
 *
 * CUT FROM THE GEOMETRY, like PlayerStaYawComponent from the place: the frame (PlayerStaFrmComponent) says
 * where the place is measured from, this row says which placement that frame belongs to.
 *
 * WRITTEN BY PlayerSimWalkSystem alone, together with PlayerStaFrmComponent. READ BY PlayerSimWalkSystem (the
 * frame it files), PlayerSimStndSystem (which answer it takes over).
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
 * @brief On which plate a walking figure was placed, and how often
 */
struct PlayerStaPlcmComponent {
    uint32_t owner = 0;   // the area plate entity the frame is the disc of; InvalidEntityId for a chunk-corner frame
    uint32_t seq = 0;     // placement count, raised each time the figure is put somewhere
};

}  // namespace ase::player
