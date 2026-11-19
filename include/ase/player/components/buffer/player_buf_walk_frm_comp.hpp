#pragma once

/**
 * ASE ECS COMPONENT (BUFFER)
 *
 * @file        player_buf_walk_frm_comp.hpp
 * @brief       PlayerBufWalkFrmComponent - the world origin of the frame a figure files its step in
 * @description Bridge component: the frame origin of a walking figure in world metres, settled by
 *              PlayerSimWalkSystem from the chunk-relative frame (PlayerStaFrmComponent) and the chunk edge,
 *              written onto the row HUB_PHYS_WALK by PlayerHubWalkSystem. ase-physics asks the elevation cells
 *              of ase-gis at this origin plus the figure's place, so the terrain it meets lies under the figure's
 *              world place.
 *
 *              buf = buffer, walk = walk, frm = frame
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    communication
 * @parity      server_only
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
 * KUERZEL AUS DER TAXONOMIE: "frame": "frm" (07_prc_process), nachgeschlagen.
 *
 * SYN PATTERN (Hub I/O vs Calculation Separation)
 *
 * This component is the BRIDGE between:
 *   - Calc System (SERVER-ONLY): PlayerSimWalkSystem computes chunk * edge + off
 *   - Hub System (SERVER-ONLY): PlayerHubWalkSystem writes it unchanged onto the row
 *
 * HUB TARGET (row HUB_PHYS_WALK on the figure entity)
 *   origin_x → HUB_PHYS_WALK_ORIGIN_X      origin_z → HUB_PHYS_WALK_ORIGIN_Z
 *
 * Unlike the notepad of a running save (PlayerBufPstComponent) both fields have their writer and their reader
 * in the same pass.
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
 * @brief The world origin of the frame a figure files its step in
 */
struct PlayerBufWalkFrmComponent {
    float origin_x = 0.0f;   // m - world place of the frame origin along the axis of PLR_POS_X
    float origin_z = 0.0f;   // m - world place of the frame origin along the axis of PLR_POS_Y
};

}  // namespace ase::player
