#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_sta_slot_comp.hpp
 * @brief       PlayerStaSlotComponent - the seat of the device player a figure belongs to
 * @description On the figure: the seat its spawn request named (HUB_PLR_SPWN_SEAT, the N of
 *              player_id seat-N). Its presence says the seat is known; a figure that entered by
 *              another way than a host spawn carries none. PlayerHubSlotPubSystem lays it and
 *              files the same seat as the per-entity hub value PLR_SEAT, which ase-hub keeps as the
 *              identity of the figure's last state across a restart (D2467).
 *
 * sta = state, slot = seat of a device player
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state
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
 * WHY server_only: the seat names the device player on the host that serves the spawn; the client
 * knows its own seat from its session and reads no seat of other figures.
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
 * @brief The seat of the device player a figure belongs to
 */
struct PlayerStaSlotComponent {
    uint32_t seat = 0;  // seat of the device player (HUB_PLR_SPWN_SEAT); seat 0 is the host itself
};

}  // namespace ase::player
