#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_walk_rplc_tag.hpp
 * @brief       PlayerWalkRplcTag - the figure was put somewhere and files a new frame with ase-physics
 * @description Request tag of a walking figure. A system that puts the figure somewhere other than where
 *              ase-physics walked it (teleport, knockback, migration, the rim of its area) sets this tag;
 *              PlayerSimWalkSystem then lays the frame again and raises the placement count, so ase-physics
 *              takes the new place and drops the old body state. Without it the next answer of ase-physics
 *              would walk the figure back to where it stood.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    tag
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
 * KUERZEL AUS DER TAXONOMIE: "walk": "walk", "replace": "rplc" (03_act_action), nachgeschlagen.
 *
 * WHY shared: it has two readers. PlayerSimWalkSystem (server_only) files the row HUB_PHYS_WALK; the shared
 * PlayerSimStndSystem excludes every figure carrying it, so the answer of ase-physics to the old placement
 * never overwrites the place the figure was just put at. The client runs the same PlayerSimStndSystem and
 * needs the type for that exclude; every writer of the tag is a server_only system, so on the client it
 * never stands and the exclude passes every figure.
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
 * [ ] Filename: player_walk_rplc_tag.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: PlayerWalkRplcTag (PascalCase)
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
 * @brief PlayerWalkRplcTag - the figure was put somewhere and files a new frame
 *
 * State: the place of the figure no longer follows from the last answer of ase-physics
 * Filter: PlayerSimWalkSystem (lays the frame again); PlayerSimStndSystem skips the figure meanwhile
 * Added: PlayerSimTlptSystem, PlayerSimKnbkSystem, PlayerMigAplySystem, PlayerSimBndySystem (beyond the rim),
 *        PlayerSimWalkSystem (a figure without a frame, or whose frame no longer fits its area)
 * Removed: PlayerSimWalkSystem, once the frame is laid
 */
struct PlayerWalkRplcTag {};

}  // namespace ase::player
