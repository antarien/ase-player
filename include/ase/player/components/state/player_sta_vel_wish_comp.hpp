#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_sta_vel_wish_comp.hpp
 * @brief       PlayerStaVelWishComponent - the speed the legs of the figure ask for in this tick
 * @description The horizontal speed a player figure wants to walk at, in metres per second along the world
 *              axes, before its ground answered. PlayerCtrlMovSystem writes it from stick, course and gait,
 *              PlayerSimChtSystem forces it in cheat mode, PlayerSimBndySystem takes its outward part at the rim,
 *              and PlayerSimWalkSystem files it as the walk request ase-physics turns into motion.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state
 * @parity      shared
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-23
 * @modified    2026-11-23
 * @version     00.01.48.00098 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * WISH AND SPEED ARE TWO VALUES. The wish is what the legs ask for; the speed (PlayerStaVelComponent) is what
 * ase-physics granted the body on its ground, laid by PlayerSimStndSystem. While both lived in one component the
 * wish of this tick overwrote the granted speed before anyone read it, so the gait, the motion row of the hub and
 * the anticheat read the stick instead of the body: a figure wading a brook showed the speed of dry ground, and a
 * figure that rolled down a bank at no wish showed it standing (register B0818).
 *
 * AXES: as PlayerStaVelComponent, Y is the vertical in this module; the wish has no vertical part, the vertical
 * speed is the answer of the ground alone.
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
 * [ ] Filename: player_sta_vel_wish_comp.hpp (module FULL, tax 3-4 chars)
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

namespace ase::player {

/**
 * @brief PlayerStaVelWishComponent - the horizontal walk wish of this tick, along the world axes
 *
 * Kept apart from the granted speed (PlayerStaVelComponent), because the two have different writers: the wish
 * comes from the stick (PlayerCtrlMovSystem), the speed from the ground (PlayerSimStndSystem). One container for
 * both let the wish erase the answer of the ground in the same tick.
 */
struct PlayerStaVelWishComponent {
    float vx = 0.0f;  // Horizontal wish, world axis X (m/s)
    float vz = 0.0f;  // Horizontal wish, world axis Z (m/s)
};

}  // namespace ase::player
