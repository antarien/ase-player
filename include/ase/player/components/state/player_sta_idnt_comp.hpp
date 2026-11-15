#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_sta_idnt_comp.hpp
 * @brief       PlayerStaIdntComponent - who this figure is and since when
 * @description The lasting identifier of a player figure together with the two time marks that
 *              span its session: when it came into being and when its last input arrived.
 *              Everything that makes a figure recognisable across restarts, region changes and
 *              reconnections stands here.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state
 * @parity      shared
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2025-12-25
 * @modified    2026-11-13
 * @version     00.00.00.00000 [seed]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * WHY THE IDENTIFIER IS A FIXED CHARACTER FIELD AND NO POINTER: 64 bytes lie far below the limit
 * from which the pointer pattern is required, and the identifier travels with the figure across
 * the region border. A pointer would be worthless there - it would point into the memory of the
 * SENDER. The migration therefore carries not the characters themselves but their FNV-1a32 hash as
 * u32 (player_mig_ser_sys.cpp), and the receiving side keeps the figure under the same number.
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
 * [ ] Large data uses pointer pattern (uint64_t ptr = 0)
 * [ ] Large data in registry.ctx()? (component has only lookup ID!)
 * [ ] Tag structs end with Tag suffix - N/A (not a tag)
 * [ ] Filename: player_sta_idnt_comp.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name derived from filename (snake_case to PascalCase)
 * [ ] 1 File = 1 Component
 * [ ] File in correct category subfolder
 * [ ] Per-entity runtime values use state/ components (NOT types.hpp!)
 * [ ] SHARED components listed in codegen.json components.shared
 * [ ] Pointer components in codegen.json components.server_only
 * [ ] Strings < 64 bytes use char[N] fixed arrays
 * [ ] Strings 64-256 bytes use appropriately sized char[N]
 * [ ] Strings > 256 bytes use Pointer Pattern (uint64_t ptr, uint16_t len)
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
 * @brief PlayerStaIdntComponent - identifier and session time marks of a player figure
 *
 * player_id     = Fixed-size player identifier (UUID format, max 36 chars + null)
 * spawned_at_ms = Timestamp in milliseconds when player spawned (steady_clock epoch)
 * last_input_ms = Timestamp in milliseconds of last input (steady_clock epoch)
 *
 * The two time marks stand BESIDE the identifier and not in a component of their own, because they
 * describe the same matter: the life span of exactly this identifier. Whoever wants to know whether
 * a figure still plays compares the last input against the clock - having both in one access is no
 * collecting container but the same subject.
 */
struct PlayerStaIdntComponent {
    char player_id[64] = {};      // Lasting identifier of the figure (UUID text, 36 chars + null)
    uint64_t spawned_at_ms = 0;   // Moment the figure came into being (ms, steady_clock epoch)
    uint64_t last_input_ms = 0;   // Last input of this figure (ms, steady_clock epoch)
};

}  // namespace ase::player
