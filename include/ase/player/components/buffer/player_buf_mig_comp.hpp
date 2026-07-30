#pragma once

/**
 * ASE ECS COMPONENT
 *
 * @file        player_buf_mig_comp.hpp
 * @brief       PlayerBufMigComponent - the serialized PlayerSnap awaiting frame-100 egress
 * @description Buffer category: the 42-byte PlayerSnap (frozen layout, ase/types/region_wire.hpp)
 *              PlayerMigSerSystem encoded from the player's live player_st_* components, staged
 *              on the player entity between the serialize (Observation, ase-player) and the send
 *              (Transmission, ase-world WorldPlrMigSndSystem wraps it into the frame-100 header
 *              and pushes it onto the transport OutboundQueue). The byte image is a single fixed
 *              attribute like a char[N] string - never entity-per-byte, never a heap pointer;
 *              at 42 bytes it stays far under the 256-byte inline ceiling.
 *
 * buf = buffer, mig = migrate
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    network/synchronization
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

#include <ase/types/region_wire.hpp>

namespace ase::player {

/**
 * PlayerBufMigComponent - one staged PlayerSnap byte image (buffer between serialize and send).
 *
 * snap_len is the encoder's receipt: exactly types::PLAYER_SNAP_SZ after a successful encode,
 * 0 while unset - the send system ships only complete images.
 */
struct PlayerBufMigComponent {
    char snap[types::PLAYER_SNAP_SZ] = {};  // serialized PlayerSnap bytes (frozen 42-byte layout, single attribute)
    uint32_t snap_len = 0;                  // encoded byte count (types::PLAYER_SNAP_SZ when staged, 0 when empty)
};

}  // namespace ase::player
