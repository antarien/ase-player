#pragma once

/**
 * ASE ECS COMPONENT
 *
 * @file        player_sta_epch_comp.hpp
 * @brief       PlayerStaEpchComponent - the player's wire identity + monotonic migrate epoch
 * @description WS-H.4 idempotency anchor. player_ref is the FNV-1a32 of the player's UUID string
 *              (PlayerStIdComponent.player_id) - the u32 identity PlayerSnap carries on the wire.
 *              epoch is the per-player monotonic migrate counter: the serializer bumps it once
 *              per armed migrate, the destination World drops any frame whose epoch is not newer
 *              than the epoch it already holds for that player_ref - a duplicate or late
 *              PLAYER_MIGRATE(100) can never instantiate a player twice. The durable copy lives
 *              in the Mongo `players` authority at the Replica; this row is the live World-side
 *              mirror seeded on spawn or on migrate-instantiate.
 *
 * sta = state, epch = epoch
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    state
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
 * PlayerStaEpchComponent - wire identity + migrate epoch of one player (WS-H.4 idempotency).
 *
 * Writers: the serializer (epoch bump on an armed migrate) and the migrate receive
 * (instantiate/update at the destination). Reader: every frame-100 decode's duplicate check.
 */
struct PlayerStaEpchComponent {
    uint32_t player_ref = 0;    // FNV-1a32 of the player UUID string (PlayerSnap wire identity)
    uint32_t player_epoch = 0;  // per-player monotonic migrate epoch (duplicates dropped by compare)
};

}  // namespace ase::player
