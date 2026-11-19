#pragma once

/**
 * ASE ECS COMPONENT (REQUEST)
 *
 * @file        player_req_chr_comp.hpp
 * @brief       PlayerReqChrComponent - the character a spawn request names for its figure
 * @description On the request entity of a spawn, beside PlayerReqSpwnComponent: the hash of the key
 *              of the character the host named for the figure (CAUSA_ASE_CREATION, which character
 *              a figure plays). Laid by PlayerHubSpwnSystem only where the spawn row names a
 *              character; PlayerHubChrPubSystem reads it together with the answer of the spawn and
 *              tells the star which character the new figure plays. A request without this row
 *              names none.
 *
 * req = request, chr = character
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    communication/request
 * @parity      server_only
 * @usecase     usecase:101557 - receive a spawn request
 * @actor       Player Presence Steward (PLAYPRESTE)
 * @created     2026-11-17
 * @modified    2026-11-17
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * WHY server_only: the request entity lives in the tier that serves the spawn and ends there.
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
 * @brief The character a spawn request names, by the hash of its key
 */
struct PlayerReqChrComponent {
    uint32_t character_hash = 0;  // hash of the key of the character the figure plays
};

}  // namespace ase::player
