#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_slot_pnd_tag.hpp
 * @brief       PlayerSlotPndTag - the seat of this answered spawn request was put on its figure
 * @description Cursor of one pass of PlayerHubSlotPubSystem: it stamps every answered spawn request
 *              whose seat it read, takes the seat row off the stamped requests in one range call
 *              and clears the cursor in the same call. The cursor never outlives a tick.
 *
 * slot = seat of a device player, pnd = pending
 *
 * META
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    tag
 * @parity      server_only
 * @usecase     usecase:101637 - seat a figure
 * @actor       Player Presence Steward (PLAYPRESTE)
 * @created     2026-11-19
 * @modified    2026-11-19
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
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
 * [ ] Large data in registry.ctx() - N/A (Tags have no data)
 * [ ] Tag structs end with Tag suffix
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
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
 * @brief PlayerSlotPndTag - cursor over the requests whose seat was read in this pass
 *
 * Added:   By PlayerHubSlotPubSystem on every answered request it read.
 * Removed: By PlayerHubSlotPubSystem at the end of the same pass (registry.clear).
 */
struct PlayerSlotPndTag {};

}  // namespace ase::player
