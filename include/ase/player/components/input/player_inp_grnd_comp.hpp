#pragma once

/**
 * ASE ECS COMPONENT (INPUT)
 *
 * @file        player_inp_grnd_comp.hpp
 * @brief       PlayerInpGrndComponent - the edges of the loaded ase-gis raster around a figure
 * @description Bridge component: on a figure bound to an area plate (row HUB_PLR_AREA), the four edges of the
 *              loaded elevation raster around it, the chunk borders ase-gis answers on HUB_GIS_GRND, filled in
 *              every Ingestion pass by PlayerAreaSynSystem once all four are measured. PlayerSimBndySystem reads
 *              it to keep the figure inside the loaded raster. A figure without it walks unbounded.
 *
 *              inp = input, grnd = loaded ground
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    input
 * @parity      server_only
 * @usecase     usecase:101612 - keep figure in loaded area
 * @actor       Game Areal Plate (AREAL)
 * @created     2026-11-18
 * @modified    2026-11-24
 * @version     00.01.45.00095 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * WHY server_only: PlayerAreaSynSystem fills it from the star, and its reader PlayerSimBndySystem is a
 * server_only system that writes the settled place before the chunk address is normalised.
 *
 * SYN PATTERN (Hub I/O vs Calculation Separation)
 *
 * This component is the BRIDGE between:
 *   - Sync System (SERVER-ONLY): PlayerAreaSynSystem reads the Hub, writes this Component
 *   - Calc System: PlayerSimBndySystem reads this Component, writes the place and the velocity
 *
 * HUB SOURCE (row HUB_GIS_GRND on the figure, the answer of ase-gis to HUB_GIS_GRND_REQ of ase-physics)
 *   HUB_GIS_GRND_WEST → west  HUB_GIS_GRND_EAST → east  HUB_GIS_GRND_SOUTH → south  HUB_GIS_GRND_NORTH → north
 * HUB SOURCE (row HUB_AVTR_RIG on the figure, the skeleton ase-pl-avatar measured)
 *   HUB_AVTR_RIG_RADIUS_M → radius, the same measure PhysicsHubWalkSyncSystem gives the capsule of the body
 *
 * THE BOX STANDS WHERE THE GLASS STANDS. PhysicsWalkBndySystem holds the centre of the capsule one radius inside
 * each edge; PlayerSimBndySystem keeps the place at least that far in, so both stop the figure at one line.
 *
 * THE AREA IS RASTER, NEVER A CIRCLE (usecase:101612, HUB_GIS_GRND_REQ). The edges run along the chunk borders of
 * the loaded elevation raster, walked by ase-gis from the chunk of the figure along its row and its column while
 * the next chunk still carries a raster; they move out when a neighbouring chunk is loaded. ase-physics holds the
 * walking body at the same edges (PhysicsWalkBndySystem), so the figure and its body meet one border.
 *
 * THE PRESENCE IS THE MEASUREMENT. The component stands only while the figure is bound and all four edges are
 * measured; the boundary is a value of the raster, never a number of the player module.
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
 * @brief The edges of the loaded ase-gis raster around a figure
 */
struct PlayerInpGrndComponent {
    float west = 0.0f;   // m - west edge of the loaded raster on the chunk row of the figure, along PLR_POS_X
    float east = 0.0f;   // m - east edge of the loaded raster on the same chunk row, along PLR_POS_X
    float south = 0.0f;  // m - south edge of the loaded raster on the chunk column of the figure, along PLR_POS_Y
    float north = 0.0f;  // m - north edge of the loaded raster on the same chunk column, along PLR_POS_Y
    float radius = 0.0f; // m - radius of the capsule ase-physics walks the figure as (HUB_AVTR_RIG_RADIUS_M); 0 while unmeasured
};

}  // namespace ase::player
