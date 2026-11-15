#pragma once

/**
 * ASE ECS COMPONENT (BUFFER)
 *
 * @file        player_buf_walk_comp.hpp
 * @brief       PlayerBufWalkComponent - the step a figure files with ase-physics this tick
 * @description Bridge component: the place and the horizontal velocity of a walking figure relative to its
 *              frame, settled by PlayerSimWalkSystem in Dynamics and written onto the row HUB_PHYS_WALK by
 *              PlayerHubWalkSystem in the same pass. ase-physics walks the body from this place with this
 *              velocity, adds gravity and ground contact, and answers on HUB_PHYS_STAND.
 *
 *              buf = buffer, walk = walk
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
 * SYN PATTERN (Hub I/O vs Calculation Separation)
 *
 * This component is the BRIDGE between:
 *   - Calc System (SERVER-ONLY): PlayerSimWalkSystem turns the chunk address into a place in the frame
 *   - Hub System (SERVER-ONLY): PlayerHubWalkSystem writes it unchanged onto the row
 *
 * HUB TARGET (row HUB_PHYS_WALK on the figure entity)
 *   x → HUB_PHYS_WALK_X      y → HUB_PHYS_WALK_Y      z → HUB_PHYS_WALK_Z
 *   vel_x → HUB_PHYS_WALK_VEL_X      vel_z → HUB_PHYS_WALK_VEL_Z
 *
 * THE FRAME IS ITS OWN ROW. The origin the place is relative to stands in PlayerBufWalkFrmComponent: the step
 * changes every tick, the frame only when the figure is placed. Unlike the notepad of a running save
 * (PlayerBufPstComponent) every field here has its writer and its reader in the same pass.
 *
 * WHY server_only: the row is written by the server; the device draws the place PlayerSimStndSystem takes
 * over from the answer and never files a step of its own.
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
 * @brief The step a figure files with ase-physics this tick
 */
struct PlayerBufWalkComponent {
    float x = 0.0f;       // m - place along the axis of PLR_POS_X, relative to the frame origin
    float y = 0.0f;       // m - height above the frame level
    float z = 0.0f;       // m - place along the axis of PLR_POS_Y, relative to the frame origin
    float vel_x = 0.0f;   // m/s - horizontal velocity along the axis of PLR_POS_X
    float vel_z = 0.0f;   // m/s - horizontal velocity along the axis of PLR_POS_Y
};

}  // namespace ase::player
