/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sync_tlpt_sys.cpp
 * @brief       PlayerSyncTlptSystem - Sync the backend teleport lever into its bridge component
 * @description SYN PATTERN: reads PLR_CHEAT_TELEPORT into PlayerInpTlptComponent; the calc system
 *              PlayerSimTlptSystem applies the jump from there without any Hub access.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    input
 * @schedule    Integration
 * @created     2026-10-03
 * @modified    2026-10-03
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_SYNC_TLPT: backend teleport lever → bridge component)
 *
 *   [Hub "PLR_CHEAT_TELEPORT" — set by WorldPlrChtRcvSystem from frame 76 (owner = hash(player_id))]
 *          │
 *          ▼
 *   ┌──────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSyncTlptSystem (SERVER-ONLY)          │
 *   │                                                          │
 *   │  READS:                                                  │
 *   │    → PlayerStaIdntComponent (player identity)            │
 *   │    → Hub "PLR_CHEAT_TELEPORT", "PLR_PROJECT_HASH_HI/LO"  │
 *   │                                                          │
 *   │  WRITES:                                                 │
 *   │    → PlayerInpTlptComponent.jump_m (bridge)              │
 *   │    → Hub "PLR_PROJECT_HASH_HI/LO" (entity owner)         │
 *   └──────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   PlayerSimTlptSystem (Dynamics) adds the jump to the position before the physics step
 *
 * A PLAYER WITHOUT A LEVER GETS NO BRIDGE ROW. The component appears with the first lever value
 * and stays; a cleared lever writes 0, which the calc system reads as "no jump". The lever's
 * absence is therefore never confused with a jump of zero metres. Unlike PlayerSyncInpSystem, an
 * absent value is not mapped to a rest value here — there is no rest value of a lever, only its
 * absence.
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1)):
 *
 * READS (from Hub):
 *   "PLR_CHEAT_TELEPORT"_hs        → per-tick jump in metres (owner = hash(player_id))
 *   "PLR_PROJECT_HASH_HI/LO"_hs    → the lever's project label (same owner)
 *
 * WRITES (to Hub):
 *   "PLR_PROJECT_HASH_HI/LO"_hs    → republished onto the player ENTITY owner while a jump is set
 *                                    (same seam as PlayerSimChtSystem and PlayerSimEcoSystem)
 *
 * ECS SYSTEM IMPLEMENTATION COMPLIANCE
 *
 * [ ] Layer dependencies checked (only depend on lower layers)
 * [ ] Existing functions checked (ase-math, ase-utils, ase-containers)
 * [ ] Abbreviations defined in types.hpp or documentation
 * [ ] types.hpp created with all constants and enums
 * [ ] STATELESS? No member variables?
 * [ ] Views created on demand, not stored?
 * [ ] NO direct calls to other systems?
 * [ ] Communication only via Components?
 * [ ] Helpers in anonymous namespace (NOT static!)?
 * [ ] Math functions from ase-math (Layer 0)?
 * [ ] NO file-level static/constexpr?
 * [ ] Registered in Module with correct Schedule?
 * [ ] Filename matches convention?
 * [ ] Class name derived correctly from filename?
 * [ ] Using Deferred Deletion Pattern? (Tag + Batch Destroy)
 * [ ] NO destroy() on other entities during iteration?
 * [ ] Cleanup System in Schedule::Conclusion?
 * [ ] NO local arrays/vectors for collection?
 * [ ] 1 File = 1 System?
 * [ ] Folder structure matches convention?
 * [ ] components/, systems/, src/ have IDENTICAL subfolder structure?
 * [ ] Layer dependencies respected (no upward dependencies)?
 * [ ] NO inline nlohmann::json + .dump() in broadcast systems?
 * [ ] Serializer functions in anonymous namespace?
 * [ ] *NetBctReqSystem + *NetBctSndSystem pattern?
 * [ ] Math functions from ase-math? (lerp, clamp, noise)
 * [ ] Containers from ase-containers? (RingBuffer)
 * [ ] Types from ase-types? (Result, Option)
 * [ ] Utils from ase-utils? (UUID, hash)
 * [ ] No duplicate functionality across modules?
 * [ ] ONLY primitive types: int, float, uint32_t, bool, etc.
 * [ ] ONLY ase-math for math (NO std::min, std::max, std::clamp!)
 * [ ] ONLY ase-containers for containers (NO std::vector, std::map, std::unordered_map!)
 * [ ] ONLY ase-types for Result/Option (NO std::optional, std::expected!)
 * [ ] std:: FORBIDDEN except: <cstdint>, <cmath> basics, <cassert>
 * [ ] CAUSAL CHAIN documented (Input → Processing → Output)
 * [ ] HUB Pattern documented (READS/WRITES)
 * [ ] hub::get() for reads
 * [ ] hub::set() for writes
 * [ ] Method order: on_start → tick → on_stop
 * [ ] ALL THREE METHODS implemented
 * [ ] on_start/on_stop: log::debug with system name
 * [ ] log::warn() if value EXISTS but invalid (e.g., health < 0, temp > 1000)
 * [ ] log::error() for EVERY NOT_FOUND check (see ase-log/log.hpp ERR::CAT::*)
 * [ ] Unused params: (void)dt; or commented parameter name
 * [ ] NO switch/case statements? (use Tag-filtered Views (separate View per type)!)
 * [ ] NO if-else chains for type dispatch? (use separate Systems per type!)
 * [ ] NO instanceof/dynamic_cast checks? (use Tags for entity classification!)
 * [ ] NO factory patterns with type enums? (use Component composition!)
 * [ ] NO inheritance hierarchies? (use Component composition!)
 * [ ] NO virtual dispatch for game logic? (only ecs::System base class allowed!)
 * [ ] NO singleton patterns? (use Manager Tags on entities!)
 * [ ] NO state machines with switch? (use Tag-based state + separate Systems!)
 * [ ] ALL behavior driven by Component DATA, not hardcoded logic?
 * [ ] NO hardcoded entity types? (types defined by Component composition!)
 * [ ] NO hardcoded processing order? (order via Schedule + run_after!)
 * [ ] NO hardcoded value ranges? (ranges in types.hpp constants!)
 * [ ] NO hardcoded special cases? (special cases = Tags + dedicated Systems!)
 * [ ] Formulas use Component fields, not magic numbers?
 * [ ] New behavior = new Component + new System, NOT if-else in existing code?
 * [ ] NO `find_*()` with View/Query? (use DUAL-PATTERN)
 * [ ] NO `check_*()`/`has_*()`/`is_*()` with View/Query? (use DUAL-PATTERN)
 * [ ] NO `get_*()` with View/Query? (use DUAL-PATTERN)
 * [ ] NO struct in namespace {}? (use Component)
 * [ ] NO collect-then-process? (use single-pass)
 * [ ] NO View/Query in Helper? (only pure math)
 * [ ] NO `bool has_*` for type categories in Components? (use Tags!)
 * [ ] NO `bool is_*` for type categories in Components? (use Tags!)
 * [ ] NO `uint8_t *_type` field with if-chain dispatch? (use Tag-filtered Views!)
 * [ ] Type determined by Tag composition, not boolean field?
 * [ ] N-item support via Entity-per-Item + Tags, not type booleans?
 * [ ] Tag-filtered Views per type, not if-chain in single loop?
 * [ ] NO Entity-per-Character pattern when loading strings?
 * [ ] String loading uses char[N] fixed arrays or Pointer Pattern?
 * [ ] String hashing via entt::hashed_string for lookup keys?
 * [ ] String data stored as single attribute, not per-character entities?
 * [ ] NO std::shared_ptr in Components? (use Flyweight Pattern!)
 * [ ] NO void* in Components? (use Flyweight Pattern!)
 * [ ] NO static std::unordered_map for resource storage? (use ResourceManager via ctx!)
 * [ ] External resources (shared_ptr, handles) accessed via registry.ctx().get<ResourceManager&>()?
 * [ ] ResourceManager registered in on_start() via registry.ctx().emplace<ResourceManager&>()?
 * [ ] Components store ONLY uint32_t IDs referencing external resources?
 */

// INCLUDES - ONLY THESE ARE ALLOWED!
// FORBIDDEN: <vector>, <map>, <unordered_map>, <optional>, <algorithm>, <chrono>
// ALLOWED:   <cstdint>, <cmath>, <cassert>, ase-* headers

// Own header FIRST
#include <ase/player/systems/sync/player_sync_tlpt_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/input/player_inp_tlpt_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// ase-types (Layer 0) for NOT_FOUND / positive-float SSOT predicates
#include <ase/types/types.hpp>
// Hub for O(1) API
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

namespace ase::player {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

// No helper functions needed - one Hub read per player into one bridge field.

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerSyncTlptSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSyncTlptSystem] Started");
}

void PlayerSyncTlptSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto view = registry.view<PlayerStaIdntComponent>();

    for (auto [entity, id] : view.each()) {
        uint32_t plr_owner = entt::hashed_string(id.player_id).value();

        float jump = hub::get(registry, plr_owner, "PLR_CHEAT_TELEPORT"_hs);
        if (ase::types::is_not_found(jump)) {
            continue;  // no teleport lever was ever set for this player
        }

        auto& inp = registry.get_or_emplace<PlayerInpTlptComponent>(entity);
        const float prev_jump = inp.jump_m;  // 0.0f on the first (zero-init) emplace
        const float applied = ase::types::is_pos_float(jump) ? jump : 0.0f;
        inp.jump_m = applied;

        // Republish the player's session-scoped project label onto the player ENTITY owner, so the
        // trigger PlayerAccMovSystem raises is attributed to the customer project.
        if (ase::types::is_pos_float(applied)) {
            uint32_t owner = static_cast<uint32_t>(entity);
            float proj_hi = hub::get(registry, plr_owner, "PLR_PROJECT_HASH_HI"_hs);
            if (!ase::types::is_not_found(proj_hi)) {
                float proj_lo = hub::get(registry, plr_owner, "PLR_PROJECT_HASH_LO"_hs);
                if (!ase::types::is_not_found(proj_lo)) {
                    hub::set(registry, owner, "PLR_PROJECT_HASH_HI"_hs, proj_hi);
                    hub::set(registry, owner, "PLR_PROJECT_HASH_LO"_hs, proj_lo);
                }
            }
        }

        // Log ONLY on the induction change (rising or falling), never per sustained tick.
        if (applied != prev_jump) {
            log::info("[PlayerSyncTlptSystem] induced teleport jump {} → {} m per tick (player_owner={})",
                      prev_jump, applied, plr_owner);
        }
    }
}

void PlayerSyncTlptSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSyncTlptSystem] Stopped");
}

}  // namespace ase::player
