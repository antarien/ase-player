#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_sta_frm_comp.hpp
 * @brief       PlayerStaFrmComponent - the origin and level a walking figure files its place from
 * @description The frame of a figure that walks as a body of ase-physics: its place on the row HUB_PHYS_WALK
 *              is measured from this origin and above this level. A figure bound to an area plate walks in
 *              the frame of that plate (the centre of its levelled disc and its level); a figure bound to no
 *              plate walks in the frame of a chunk corner at level 0. The origin is held chunk-relative, like
 *              PlayerStaPosComponent, so no world coordinate grows beyond what a float carries.
 *
 *              sta = state, frm = frame
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
 * KUERZEL AUS DER TAXONOMIE: "frame": "frm" (07_prc_process), nachgeschlagen; 28_net_network fuehrt "fram"
 * fuer den Datenrahmen eines Protokolls, das ist hier nicht gemeint.
 *
 * GEOMETRY ONLY, like the cut of PlayerStaYawComponent ("view is not place"): which plate the frame belongs
 * to and how often the figure was placed stand in PlayerStaPlcmComponent. The two change together, but a
 * reader of the geometry (PlayerSimStndSystem turning the answer of ase-physics back into the chunk address)
 * needs neither.
 *
 * WRITTEN BY PlayerSimWalkSystem alone, when the figure is placed (PlayerWalkRplcTag). WORLD ORIGIN =
 * chunk * edge + off on each axis, the edge being TER_CONST_CHUNK_SIZE on the manager row
 * (PlayerInpChnkCnstComponent).
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
 * @brief The origin and level a walking figure files its place from
 */
struct PlayerStaFrmComponent {
    int32_t chunk_x = 0;    // chunk of the frame origin along the axis of PLR_POS_X
    int32_t chunk_z = 0;    // chunk of the frame origin along the axis of PLR_POS_Y
    float off_x = 0.0f;     // m - origin within that chunk along the axis of PLR_POS_X
    float off_z = 0.0f;     // m - origin within that chunk along the axis of PLR_POS_Y
    float level = 0.0f;     // m - world height the place is measured from
};

}  // namespace ase::player
