#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT
 * =============================================================================
 *
 * @file        player_req_roam_comp.hpp
 * @brief       The errand a spawn request carries - the speed the new player is to walk at
 * @description SERVER-ONLY companion of a spawn request. It sits on the SAME request entity the
 *              spawn request sits on, and it is there only when the caller asked for a walker.
 *              A spawn WITHOUT an errand is therefore the plain spawn it always was - composition
 *              says it, not a sentinel value in the position row, because a zero speed in a
 *              spawn request would be indistinguishable from a caller that forgot the field.
 *
 * -----------------------------------------------------------------------------
 * META
 * -----------------------------------------------------------------------------
 * @module      ase-player
 * @layer       3 (Module)
 * @category    communication/request
 * @created     2026-08-09
 * @modified    2026-08-09
 * @version     1.0.0
 * @author      Jan Ohlmann (ADG/ASE/AOW)
 *
 * -----------------------------------------------------------------------------
 * ECS COMPONENT COMPLIANCE
 * -----------------------------------------------------------------------------
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
 *
 * =============================================================================
 */

namespace ase::player {

/**
 * @brief The errand of a spawn request - the speed the new player is to walk at (SERVER-ONLY).
 *
 * speed is metres per second as the caller asked for it, unjudged: PlayerLifeSpwnSystem is where
 * it meets the movement authority and becomes an input magnitude and a gear. Present only on
 * spawn requests that asked for a walker.
 */
struct PlayerReqRoamComponent {
    float speed = 0.0f;  // Walking speed the caller asked for (m/s), before the authority is applied
};

}  // namespace ase::player
