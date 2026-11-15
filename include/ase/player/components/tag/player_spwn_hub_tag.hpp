#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_spwn_hub_tag.hpp
 * @brief       PlayerSpwnHubTag - this spawn request came from a hub row of the embedding host
 * @description Marks the request entity PlayerHubSpwnSystem files for a row HUB_PLR_SPWN_REQ. The
 *              system that files the request is the one that removes it once it is answered, and
 *              this mark is how it knows its own requests among those of the network reception.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    tag
 * @parity      server_only
 * @usecase     usecase:101557 - receive a spawn request
 * @actor       Player Presence Steward (PLAYPRESTE)
 * @created     2026-11-13
 * @modified    2026-11-13
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
 * [ ] Large data uses pointer pattern - N/A (no data)
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
 * PlayerSpwnHubTag - a spawn request filed from HUB_PLR_SPWN_REQ.
 *
 * Writer: PlayerHubSpwnSystem, beside PlayerReqSpwnComponent on the request entity.
 * Reader: PlayerHubSpwnSystem, which destroys the answered request in its next pass.
 */
struct PlayerSpwnHubTag {};

}  // namespace ase::player
