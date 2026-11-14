#pragma once

/**
 * ASE ECS COMPONENT (INPUT)
 *
 * @file        player_inp_area_comp.hpp
 * @brief       PlayerInpAreaComponent - the loaded area a figure walks on
 * @description Bridge component: on a figure bound to an area plate (row HUB_PLR_AREA), the level, the
 *              radius and the world centre of the disc ase-terrain levelled for that plate, filled in every
 *              Ingestion pass by PlayerAreaSynSystem once all four values are measured, together with the plate
 *              they belong to. PlayerSimBndySystem reads it to keep the figure inside the area;
 *              PlayerSimWalkSystem lays the walking frame on the plate centre and level and names the plate as
 *              the owner ase-physics walks the body on. A figure without the row walks unbounded.
 *
 *              inp = input, area = loaded area
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    input
 * @parity      server_only
 * @usecase     usecase:101612 - keep figure in loaded area
 * @actor       Game Areal Plate (AREAL)
 * @created     2026-11-13
 * @modified    2026-11-14
 * @version     1.0.0
 *
 * WHY server_only: PlayerAreaSynSystem fills it from the star, and its reader PlayerSimBndySystem is a
 * server_only system that writes the settled place before the chunk address is normalised.
 *
 * SYN PATTERN (Hub I/O vs Calculation Separation)
 *
 * This component is the BRIDGE between:
 *   - Sync System (SERVER-ONLY): PlayerAreaSynSystem reads the Hub, writes this Component
 *   - Calc System: PlayerSimBndySystem reads this Component, writes the place and the velocity
 *
 * HUB SOURCE (owner = HUB_PLR_AREA_PLATE of the figure, all four measured)
 *   TRN_FLAT_HGT → level      TRN_FLAT_RAD → radius_m      TRN_FLAT_X → x      TRN_FLAT_Z → z
 *   HUB_PLR_AREA_PLATE of the figure → plate
 *
 * THE PRESENCE IS THE MEASUREMENT. The row stands only while the plate is bound and its disc is measured;
 * the boundary is a value of the area, never a number of the player module.
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
 * @brief The loaded area a figure walks on
 */
struct PlayerInpAreaComponent {
    float level = 0.0f;      // m - world height the area is levelled to
    float radius_m = 0.0f;   // m - radius of the area around its centre
    float x = 0.0f;          // m - world centre of the area along the axis of PLR_POS_X
    float z = 0.0f;          // m - world centre of the area along the axis of PLR_POS_Y
    uint32_t plate = 0;      // the area plate entity the disc belongs to (HUB_PLR_AREA_PLATE)
};

}  // namespace ase::player
