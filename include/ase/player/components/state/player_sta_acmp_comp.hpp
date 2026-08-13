#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT
 * =============================================================================
 *
 * @file        player_sta_acmp_comp.hpp
 * @brief       Anti-cheat previous-position state — last observed player position for teleport detection
 * @description SERVER-ONLY: the previous horizontal position PlayerAccMovSystem observed for this player
 *              (Phase 13 / Task 13.10). The detector compares the per-tick position delta against the engine
 *              teleport authority (PLR_AC_TELEPORT_STEP); a jump larger than any legitimate single-tick move
 *              is a teleport-hack and flags the contract trigger PLAYER_MOVEMENT_SUSPICIOUS. The baseline
 *              validity is carried by the PlayerAcmpRdyTag (a Tag, not a boolean field), so the very first
 *              observed tick records the baseline without false-positiving.
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

#include <cstdint>

namespace ase::player {

/**
 * @brief Anti-cheat previous-position state for teleport detection (SERVER-ONLY).
 *
 * last_x / last_z is the horizontal position PlayerAccMovSystem observed on the prior tick. The baseline is
 * only meaningful once PlayerAcmpRdyTag is present (set on the first observed tick), so a freshly-spawned
 * player is never flagged before a baseline exists.
 */
struct PlayerStaAcmpComponent {
    int32_t last_chunk_x = 0;   // Wabenadresse X der letzten Beobachtung (exakt, S2b 2026-08-11)
    int32_t last_chunk_z = 0;   // Wabenadresse Z der letzten Beobachtung (exakt)
    float last_local_x = 0.0f;  // Meter in der Wabe X der letzten Beobachtung
    float last_local_z = 0.0f;  // Meter in der Wabe Z der letzten Beobachtung
};

}  // namespace ase::player
