#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT
 * =============================================================================
 *
 * @file        player_sta_roam_comp.hpp
 * @brief       The errand of a backend-driven player — heading, leg clock and input magnitude
 * @description SERVER-ONLY state on a REAL ase-player entity the operator set walking. It carries
 *              the walking speed the spawn frame asked for, the heading of the current leg, the
 *              seconds left of that leg and the forward magnitude the hub writer publishes. The
 *              two phases of a journey are TAGS beside this row, never a code in it: walking is
 *              PlayerRoamWandTag, pausing is PlayerRoamRestTag, and one system serves each. No
 *              second actor type is created anywhere - the entity is the same real player a human
 *              client would drive, fed through the same PLR_INP_* seam.
 *
 * -----------------------------------------------------------------------------
 * META
 * -----------------------------------------------------------------------------
 * @module      ase-player
 * @layer       3 (Module)
 * @category    state
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

#include <cstdint>

namespace ase::player {

/**
 * @brief Errand state of a real ase-player entity the backend drives on foot (SERVER-ONLY).
 *
 * speed is the metres per second the spawn frame asked for. forward is that speed expressed
 * against the gear the errand is walked in, so a walker can reach the sanctioned speed and never
 * pass it; it is computed ONCE, when the errand is born, and stands unchanged for the whole
 * journey. WHICH gear that is is not a field here but the tag PlayerRoamRunTag, decided in the
 * same moment - a gear is a category, and a category is a tag. Whether the magnitude is published
 * at all is likewise not a field: the phase tags say it, and the hub writer publishes zero for
 * the resting view. heading is the course the walk holds for the rest of its leg and travels as
 * PLR_CAM_YAW, because yaw is what the movement system turns an input into a direction with.
 * leg_sec runs the current phase down and leg_index is what the deterministic draw of the NEXT
 * leg is mixed from, so a replay walks the identical journey.
 */
struct PlayerStaRoamComponent {
    float speed = 0.0f;      // Walking speed the spawn frame asked for (m/s)
    float heading = 0.0f;    // Course of the current leg (radians), published as PLR_CAM_YAW
    float leg_sec = 0.0f;    // Seconds left of the current phase, walking or resting
    float forward = 0.0f;    // Forward input magnitude of the errand, published while walking
    uint32_t leg_index = 0;  // Legs begun so far - the draw axis of the next heading and rest
};

}  // namespace ase::player
