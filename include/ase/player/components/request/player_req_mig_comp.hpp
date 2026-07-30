#pragma once

/**
 * ASE ECS COMPONENT
 *
 * @file        player_req_mig_comp.hpp
 * @brief       PlayerReqMigComponent - boundary-migrate request on one player entity
 * @description Emplaced on the player entity by the World boundary watch (WorldPlrBndSystem,
 *              ase-world - header-only POD write into the SAME World registry, no compiled
 *              cross-link) once the WS-H.4 hysteresis armed: the player sat past the band for the
 *              dwell time. PlayerMigSerSystem answers it by bumping the player's migrate epoch and
 *              serializing the PlayerSnap into PlayerBufMigComponent; the World send system ships
 *              frame 100 and removes the request. dst_region stays REGION_ID_NONE when the source
 *              World cannot name the neighbour region (it only knows its OWN rects) - the Replica
 *              resolves the destination from the snap position against the full assignment table.
 *
 * req = request, mig = migrate
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    communication/request
 * @created     2026-07-28
 * @modified    2026-07-28
 * @version     1.0.0
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
 * [ ] Strings > 256 bytes use registry.ctx() mit Lookup-ID?
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
 * PlayerReqMigComponent - one armed boundary migrate of this player (request, consumed once).
 *
 * Producer: the World boundary watch. Consumers: the ase-player serializer (fills the snap
 * buffer) and the World send system (ships frame 100, then removes request + buffer).
 */
struct PlayerReqMigComponent {
    uint32_t dst_region = 0;  // destination region id (REGION_ID_NONE = the Replica resolves it)
    uint32_t proj_hash = 0;   // owning project of the source region (frame-100 proj_hash field)
};

}  // namespace ase::player
