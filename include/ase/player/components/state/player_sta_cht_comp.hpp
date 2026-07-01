#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT
 * =============================================================================
 *
 * @file        player_sta_cht_comp.hpp
 * @brief       Backend cheat-simulation state — the forced superhuman speed for a simulated cheater
 * @description SERVER-ONLY marker present on a real ase-player entity that the backend drives to cheat
 *              (Phase 13 / Task 13.10). The forced horizontal speed sits above the engine movement
 *              authority (PLR_AC_MAX_HORIZONTAL_SPEED), so PlayerSimChtSystem overrides the player's
 *              capped velocity each tick and the anti-cheat detector PlayerAccMovSystem flags it. This
 *              simulates a hacked client ON the real player entity — never a server-authoritative NPC and
 *              never a fabricated trigger; the cheat is induced via the /api/player/cheat backend lever.
 *
 * -----------------------------------------------------------------------------
 * META
 * -----------------------------------------------------------------------------
 * @module      ase-player
 * @layer       3 (Module)
 * @category    state
 * @created     2026-06-30
 * @modified    2026-06-30
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
 * @brief Backend cheat-simulation state for a real ase-player entity (SERVER-ONLY).
 *
 * Present only while the backend drives this player to cheat. forced_speed is the superhuman horizontal
 * speed PlayerSimChtSystem writes into the player's velocity each tick (above PLR_AC_MAX_HORIZONTAL_SPEED),
 * so the realised velocity exceeds the engine movement authority and PlayerAccMovSystem flags it.
 */
struct PlayerStaChtComponent {
    float forced_speed = 0.0f;  // Forced horizontal cheat speed (m/s); above PLR_AC_MAX_HORIZONTAL_SPEED
};

}  // namespace ase::player
