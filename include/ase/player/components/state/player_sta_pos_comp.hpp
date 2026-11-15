#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_sta_pos_comp.hpp
 * @brief       PlayerStaPosComponent - where the figure stands, relative to its chunk
 * @description The place of a player figure as a chunk address plus metres inside the chunk. The
 *              chunk it stays in, the region in charge of it, the terrain stream around it and
 *              every colour trace its path leaves on the geoid are derived from this value.
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
 * CHUNK-RELATIVE PLACE: an absolute float32 axis in world metres does not carry the address
 * space. At 204 million metres the float32 step is 16 metres, and every 0.04 m step of the physics
 * (taken back by PlayerSimStndSystem) would vanish. The chunk address is therefore an exact int32 (it carries the
 * face encoding without loss), and the metres inside the chunk are a small float with full
 * precision. ase-spatial lays its SpatialIdxPosComponent out the same way.
 *
 * The yaw is not part of the place: the view direction lives in PlayerStaYawComponent.
 *
 * THE WIRE: PlayerSnap (ase/types/region_wire.hpp, 4x f32) carries world metres. Serialisers form
 * chunk * chunk edge + local, receivers split it by floor and remainder (as in
 * character_life_spwn_sys.cpp). For sums of chunk edges the f32 metres are exact, because
 * multiples of the edge below 2^28 lie on mantissa bits.
 *
 * AXES: in this module Y is the vertical - the physics writes y from the falling speed, and X and
 * Z span the chunk grid. On the geoid and GIS side Z is the height axis. A value carried between
 * both sides has its axes swapped on the way.
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
 * [ ] Filename: player_sta_pos_comp.hpp (module FULL, tax 3-4 chars)
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
 * @brief PlayerStaPosComponent - place in this tick, relative to the chunk
 *
 * The chunk address is exact (int32), the metres inside the chunk are small and carry full float
 * precision - a step never vanishes, wherever on the planet the figure stands.
 */
struct PlayerStaPosComponent {
    int32_t chunk_x = 0;    // Chunk address X (exact, carries the face encoding without loss)
    int32_t chunk_z = 0;    // Chunk address Z (exact)
    float local_x = 0.0f;   // Metres inside the chunk along X, [0, chunk edge)
    float local_z = 0.0f;   // Metres inside the chunk along Z, [0, chunk edge)
    float y = 0.0f;         // World metres along the vertical
};

}  // namespace ase::player
