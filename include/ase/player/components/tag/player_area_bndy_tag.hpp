#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_area_bndy_tag.hpp
 * @brief       PlayerAreaBndyTag - the figure stands on the rim of its loaded area
 * @description State tag of a bound figure. PlayerSimBndySystem sets it in the frame it sets the figure back
 *              onto the chunk borders of its loaded raster and takes it off in the frame the figure stands inside again;
 *              PlayerHubMotnSystem publishes it as the mark HUB_PLR_MOTN_BNDY on the motion row, ase-pl-avatar
 *              files it on the camera record of the figure (edge) and the drawing host writes the line
 *              RAND DES AREALS of the mockup (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.7, DoD 07-C and 07-G,
 *              HUD field cam.bound).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    tag
 * @parity      server_only
 * @design      DSGN_019
 * @usecase     usecase:101612 - keep figure in loaded area
 * @actor       Game Areal Plate (AREAL)
 * @created     2026-11-17
 * @modified    2026-11-17
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * WHY server_only: its writer PlayerSimBndySystem and its reader PlayerHubMotnSystem are both server_only;
 * the rim state reaches the client as the mark of the star, never as this type.
 *
 * ECS TAG COMPLIANCE
 *
 * [ ] DATA fields ONLY - No methods (empty struct for tags)
 * [ ] NO .cpp file - Header-only
 * [ ] ONLY zero-initialization - N/A (no fields)
 * [ ] No magic numbers in defaults - N/A (no fields)
 * [ ] Entity references - N/A (no fields)
 * [ ] Single responsibility - N/A (marker only)
 * [ ] No God-Component - N/A (no fields)
 * [ ] Large data uses pointer pattern - N/A (no data)
 * [ ] Large data in registry.ctx() - N/A (Tags have no data)
 * [ ] Tag structs end with Tag suffix
 * [ ] Filename: player_area_bndy_tag.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: PlayerAreaBndyTag (PascalCase)
 * [ ] Struct name: Remove tag_ from middle, add Tag suffix
 * [ ] 1 File = 1 Component
 * [ ] File in tag/ subfolder (with optional deeper hierarchy)
 * [ ] Per-entity runtime values use state/ components (NOT types.hpp!)
 * [ ] SHARED components listed in codegen.json components.shared
 * [ ] Pointer components in codegen.json components.server_only
 * [ ] Tag replaces `bool is_*` or `bool has_*` field in Component
 * [ ] Tag replaces `uint8_t *_type` field with if-chain dispatch
 * [ ] Systems use View filter instead of if-else inside loop
 * [ ] INCLUDE: registry.view<Component, ThisTag>()
 * [ ] EXCLUDE: registry.view<Component>(entt::exclude<ThisTag>)
 * [ ] NO if (entity.has<Tag>) inside loop - use filtered View!
 * [ ] NO switch/case on type - use separate View per Tag!
 * [ ] Each state = separate Tag + separate View in System
 * [ ] N-item support via Entity-per-Item + Tags
 */

namespace ase::player {

/**
 * @brief PlayerAreaBndyTag - the figure stands on the rim of its loaded area
 *
 * State: PlayerSimBndySystem set the figure back onto, or holds it on, a chunk border of its loaded raster in this frame
 * Filter: PlayerHubMotnSystem tags HUB_PLR_MOTN_BNDY on figures with it and untags it on the others
 * Added: PlayerSimBndySystem, beyond a border or within PLR_AREA_RIM_M of it
 * Removed: PlayerSimBndySystem, free inside the four borders
 */
struct PlayerAreaBndyTag {};

}  // namespace ase::player
