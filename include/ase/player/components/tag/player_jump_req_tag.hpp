#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_jump_req_tag.hpp
 * @brief       PlayerJumpReqTag - the figure asks to jump this tick
 * @description Request tag of a player figure. The steering decides THAT the figure jumps (the jump input
 *              while it stands on the ground); ase-physics decides HOW: the impulse goes onto the row
 *              HUB_PHYS_WALK as HUB_PHYS_WALK_JUMP, and PhysicsWalkSterSystem lifts the body only while it
 *              stands on its ground. The request lives for one tick.
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
 * KUERZEL AUS DER TAXONOMIE: "jump": "jump", "request": "req", nachgeschlagen.
 *
 * WHY A TAG AND NOT THE VELOCITY. Before 07.13 the steering wrote the impulse into vy itself; vy is now the
 * answer of ase-physics (PlayerSimStndSystem takes HUB_PHYS_STAND_VEL_Y over), so a second writer would be
 * overwritten one tick later. The tag carries the wish to the one writer of the vertical motion.
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
 * [ ] Filename: player_jump_req_tag.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: PlayerJumpReqTag (PascalCase)
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
 * @brief PlayerJumpReqTag - the figure asks to jump this tick
 *
 * State: the jump input stood while the figure stood on its ground
 * Filter: PlayerHubWalkSystem, which files MOVEMENT_DEFAULT_JUMP_IMPULSE as HUB_PHYS_WALK_JUMP
 * Added: PlayerCtrlMovSystem, when jump is pressed and PlayerStaPhysComponent.on_ground holds
 * Removed: PlayerHubWalkSystem, once the impulse is filed on the row
 */
struct PlayerJumpReqTag {};

}  // namespace ase::player
