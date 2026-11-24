#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_body_req_tag.hpp
 * @brief       PlayerBodyReqTag - the figure was put somewhere and files its body with ase-physics
 * @description Request tag of a player figure. The figure is a body like every other (register A1698): each time it
 *              is put somewhere, it files itself once as HUB_PHYS_REG_BODY_ADD_FIGR on its own entity - mass, fall
 *              speed, make and the roughness of its soles from the constants PLR_FIG_* - and ase-physics lays the
 *              body afresh at the new place, at rest. The request lives until the row is filed.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    tag
 * @parity      shared
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-23
 * @modified    2026-11-23
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * KUERZEL AUS DER TAXONOMIE: "body": "body", "request": "req".
 *
 * WHY A TAG AND NOT THE PLACEMENT COUNT. PhysicsHubBodySystem withdraws the row once it laid the body; a row filed
 * every tick would lay the body afresh every tick and take its speed each time. The tag carries the one filing per
 * placement from the system that puts the figure somewhere to the one writer of the row.
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
 * [ ] Filename: player_body_req_tag.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: PlayerBodyReqTag (PascalCase)
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
 * @brief PlayerBodyReqTag - the figure files its body with ase-physics
 *
 * State: the figure was put somewhere and its body is not yet filed at the new place
 * Filter: PlayerHubWalkSystem, which files HUB_PHYS_REG_BODY_ADD_FIGR on the figure's entity
 * Added: PlayerSimWalkSystem, each time it raises the placement count
 * Removed: PlayerHubWalkSystem, once the row is filed
 */
struct PlayerBodyReqTag {};

}  // namespace ase::player
