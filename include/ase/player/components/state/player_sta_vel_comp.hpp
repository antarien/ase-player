#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_sta_vel_comp.hpp
 * @brief       PlayerStaVelComponent - how fast and where to the figure is moving now
 * @description The velocity of a player figure in metres per second, along the world axes. The
 *              movement intent and gravity write it, the state evaluation reads the gait from it,
 *              and the migration serialiser carries it across the region border.
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
 * THE WIRE: the three values travel as an f32 triple in PlayerSnap (ase/types/region_wire.hpp)
 * across the region border - a running figure goes on running on the other side.
 *
 * AXES: in this module Y is the vertical. vy is the vertical speed ase-physics answered for the
 * body (0 on its ground); the speed on the ground is sqrt(vx^2 + vz^2); X and Z span the chunk grid. On the
 * geoid and GIS side Z is the height axis. A value carried between both sides has its axes swapped
 * on the way.
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
 * [ ] Filename: player_sta_vel_comp.hpp (module FULL, tax 3-4 chars)
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
 * @brief PlayerStaVelComponent - the velocity of this tick, along the world axes
 *
 * Kept apart from the place (PlayerStaPosComponent) and from the intent
 * (PlayerStaInpMovComponent), because several systems write it: PlayerCtrlMovSystem steers the
 * horizontal parts, PlayerSimStndSystem takes vy from the stand answer of ase-physics, and the
 * reception of a migration sets all three at once. One container for place and velocity would
 * let these writers run against each other.
 */
struct PlayerStaVelComponent {
    float vx = 0.0f;  // Horizontal, world axis X (m/s)
    float vy = 0.0f;  // Vertical - gravity acts here (m/s)
    float vz = 0.0f;  // Horizontal, world axis Z (m/s)
};

}  // namespace ase::player
