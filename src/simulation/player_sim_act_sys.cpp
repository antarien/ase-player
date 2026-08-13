/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sim_act_sys.cpp
 * @brief       PlayerSimActSystem - Backend cheat-induction for the activity axes (actions + combat)
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-07-19
 * @modified    2026-07-19
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_SIM_ACT: backend lever → realised activity rates)
 *
 *   [Hub "PLR_CHEAT_ACTIONS" — set by ase player cheat --actions (owner = hash(player_id))]
 *          │
 *          ▼
 *   ┌──────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSimActSystem (SERVER-ONLY)            │
 *   │                                                          │
 *   │  READS:                                                  │
 *   │    → PlayerStaIdntComponent (player identity)               │
 *   │    → Hub "PLR_CHEAT_ACTIONS" (induced actions/sec)       │
 *   │                                                          │
 *   │  WRITES:                                                 │
 *   │    → PlayerStaActComponent.action_rate (realised)        │
 *   │    → PlayerStaCmbComponent.combat_rate (realised)        │
 *   └──────────────────────────────────────────────────────────┘
 *          │
 *          │ realised rates now exceed the activity authorities
 *          ▼
 *   PlayerAccActSystem flags PLAYER_ACTION_* and PlayerAccCmbSystem flags COMBAT_EVENT_*
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1)):
 *
 * READS (from Hub):
 *   "PLR_CHEAT_ACTIONS"_hs → induced actions/sec for the simulated cheater (owner = hash(player_id))
 *
 * WRITES (to Hub):
 *   (none)
 *
 * NOTE: SERVER-ONLY cheat simulation — the activity burst is induced on the REAL player entity via the
 * backend lever, never a fabricated topic inject and never a server-authoritative NPC (same honesty
 * rule as PlayerSimChtSystem). The combat rate follows the action lever at the same magnitude — one
 * Behavior-archetype lever drives both realised activity axes deterministically.
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
 * [ ] *NetBctReqSystem (Update) + *NetBctSndSystem (Replication) pattern?
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
#include <ase/player/systems/simulation/player_sim_act_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/state/player_sta_act_comp.hpp>
#include <ase/player/components/state/player_sta_cmb_comp.hpp>
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

// No helper functions needed - the induction is a single per-player Hub read + rate write.

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerSimActSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimActSystem] Started");
}

void PlayerSimActSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * Per player: the backend cheat lever (ase player cheat --actions) sets the Hub value
     * PLR_CHEAT_ACTIONS owned by hash(player_id). For each real player, resolve that owner from
     * PlayerStaIdntComponent.player_id and, when a positive induced rate is set, drive the realised
     * action_rate AND combat_rate to it — a real activity-hack on the real player entity. A cleared
     * lever (0.0) drops both realised rates back to zero so the detector flags clear.
     */
    auto view = registry.view<PlayerStaIdntComponent>();

    for (auto [entity, id] : view.each()) {
        uint32_t plr_owner = entt::hashed_string(id.player_id).value();

        float induced_rate = hub::get(registry, plr_owner, "PLR_CHEAT_ACTIONS"_hs);
        if (ase::types::is_not_found(induced_rate)) {
            continue;  // no activity cheat lever set for this player
        }

        auto& act = registry.get_or_emplace<PlayerStaActComponent>(entity);
        auto& cmb = registry.get_or_emplace<PlayerStaCmbComponent>(entity);
        const float prev_rate = act.action_rate;
        const float applied = ase::types::is_pos_float(induced_rate) ? induced_rate : 0.0f;
        act.action_rate = applied;
        cmb.combat_rate = applied;

        // Republish the player's session-scoped project label (the cheat route set it under
        // owner = hash(player_id)) onto the player ENTITY owner, so the anti-cheat flag and the
        // project label share one owner and the Replica forwarder reads both to stamp the trigger's
        // origin project — same seam as PlayerSimChtSystem, required for a pure archetype cheat
        // (no movement lever) to attribute to the customer project.
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
        if (applied != prev_rate) {
            log::info("[PlayerSimActSystem] induced activity rate {} → {} actions+combat/sec (player_owner={})",
                      prev_rate, applied, plr_owner);
        }
    }
}

void PlayerSimActSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimActSystem] Stopped");
}

}  // namespace ase::player
