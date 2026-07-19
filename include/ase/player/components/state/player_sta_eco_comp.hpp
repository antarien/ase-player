#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT
 * =============================================================================
 *
 * @file        player_sta_eco_comp.hpp
 * @brief       Realised per-player economy-transaction rate — the EconomyAuditor contract surface
 * @description SERVER-ONLY realised economy state of a real ase-player entity (Phase 13 AP-3): the
 *              realised economy transactions per second this player performs. The game simulation
 *              drives it (and the backend cheat lever PLR_CHEAT_DUPE induces it deterministically via
 *              PlayerSimEcoSystem); the detector PlayerAccEcoSystem compares it against the engine
 *              authority PLR_AC_MAX_TRANSACTION_RATE and flags the exact contract topic
 *              "ECONOMY_TRANSACTION_*" (ANTI_CHEAT_COMPONENT_CONTRACT.md) consumed by EconomyAuditor.
 *
 * -----------------------------------------------------------------------------
 * META
 * -----------------------------------------------------------------------------
 * @module      ase-player
 * @layer       3 (Module)
 * @category    state
 * @created     2026-07-19
 * @modified    2026-07-19
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
 * @brief Realised per-player economy-transaction rate (SERVER-ONLY, Phase 13 AP-3).
 *
 * transaction_rate is the realised economy transactions per second this player performs. Driven by
 * the game simulation (or induced by the PLR_CHEAT_DUPE backend lever via PlayerSimEcoSystem); read
 * by PlayerAccEcoSystem against PLR_AC_MAX_TRANSACTION_RATE toward the "ECONOMY_TRANSACTION_*"
 * contract topic.
 */
struct PlayerStaEcoComponent {
    float transaction_rate = 0.0f;  // Realised economy transactions/sec; above PLR_AC_MAX_TRANSACTION_RATE = suspicious
};

}  // namespace ase::player
