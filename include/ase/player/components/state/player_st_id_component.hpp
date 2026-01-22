#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_st_id_component.hpp
 * @brief       PlayerStIdComponent - Player identity data
 * @description Player ID and session timestamps
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    state
 * @created     2025-12-25
 * @modified    2026-01-22
 * @version     1.1.0
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
 * [ ] Tag structs end with Tag suffix - N/A (not a tag)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name derived from filename (snake_case to PascalCase)
 * [ ] 1 File = 1 Component
 * [ ] File in correct category subfolder
 * [ ] SHARED components listed in codegen.json components.shared
 * [ ] Pointer components in codegen.json components.server_only
 * [ ] Strings < 64 bytes use char[N] fixed arrays
 * [ ] Strings 64-256 bytes use appropriately sized char[N]
 * [ ] Strings > 256 bytes use Pointer Pattern (uint64_t ptr, uint16_t len)
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
 * player_id     = Fixed-size player identifier (UUID format, max 36 chars + null)
 * spawned_at_ms = Timestamp in milliseconds when player spawned (steady_clock epoch)
 * last_input_ms = Timestamp in milliseconds of last input (steady_clock epoch)
 */
struct PlayerStIdComponent {
    char player_id[64] = {};
    uint64_t spawned_at_ms = 0;
    uint64_t last_input_ms = 0;
};

}  // namespace ase::player
