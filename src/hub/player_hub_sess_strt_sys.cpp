/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_sess_strt_sys.cpp
 * @brief       PlayerHubSessStrtSystem - Announce each player's session start as a one-pass pulse
 * @description Producer of the contract topic PLAYER_SESSION_START (register D0703): up for one
 *              dissemination pass, down in the next, once per player entity.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    hub
 * @schedule    Observation
 * @created     2026-10-03
 * @modified    2026-10-03
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_HUB_SESS_STRT: new player → session-start trigger)
 *
 *   [PlayerStaIdntComponent + PlayerStaPosComponent — a live player this system has not seen]
 *          │
 *          ▼
 *   ┌──────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubSessStrtSystem (SERVER-ONLY)       │
 *   │                                                          │
 *   │  PASS N:   raise  → Hub PLAYER_SESSION_START = 1          │
 *   │                     + PlayerSessStrtTag                   │
 *   │  PASS N+1: lower  → Hub PLAYER_SESSION_START = 0          │
 *   │                     + PlayerSessDoneTag                   │
 *   └──────────────────────────────────────────────────────────┘
 *          │
 *          │ HubSndRplcSystem (Dissemination) ships each edge in its own pass
 *          ▼
 *   Replica ReplicaHubIgstSystem: row on the rising edge, gone on the falling edge →
 *   ReplicaAccFwdSystem fires BIN_MSG_RSN_TRIGGER_BATCH once → CoordinationDetector
 *
 * THE ORDER INSIDE ONE PASS IS THE POINT: lower first, then raise. A player raised in this pass
 * must not be lowered in the same pass — its 1 has not left yet. A player raised in the previous
 * pass has: Dissemination (70) ran between the two Observation passes (72) and shipped it.
 * The tags change after both walks (collect first, act after): each view filters on the tag the
 * pass hands out, so handing it out inside the walk would change the walked set.
 *
 * NO PROJECT LABEL HERE. The project of the trigger is the project of the region the player
 * stands in, resolved by the Replica forwarder; this tier does not know it at spawn time.
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1)):
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   "PLAYER_SESSION_START"_hs → 1 for one pass, then 0 (owner = player entity, the same owner the
 *                               anti-cheat detectors flag under)
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
#include <ase/player/systems/hub/player_hub_sess_strt_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/tag/player_sess_strt_tag.hpp>
#include <ase/player/components/tag/player_sess_done_tag.hpp>
// Hub for O(1) API
#include <ase/hub/api.hpp>
// The entities whose tag changes after the walk (collect first, act after)
#include <ase/containers/vector.hpp>
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

// No helper functions needed - two tag-filtered views, one Hub write each.

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerHubSessStrtSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubSessStrtSystem] Started");
}

void PlayerHubSessStrtSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * LOWER FIRST: every player raised in an earlier pass. Its 1 left in the Dissemination pass that
     * ran between that pass and this one; the 0 written now leaves in the next.
     */
    ase::containers::Vector<ecs::Entity> lowered;
    for (auto [entity] : registry.view<PlayerSessStrtTag>(entt::exclude<PlayerSessDoneTag>).each()) {
        hub::set(registry, static_cast<uint32_t>(entity), "PLAYER_SESSION_START"_hs, 0.0f);
        lowered.push_back(entity);
    }

    /**
     * THEN RAISE: every live, positioned player not yet announced — the same population the
     * session register publishes. One info line per player and session, never per pass.
     */
    ase::containers::Vector<ecs::Entity> raised;
    for (auto [entity, id, pos] :
         registry.view<PlayerStaIdntComponent, PlayerStaPosComponent>(
                     entt::exclude<PlayerSessStrtTag>).each()) {
        (void)pos;  // selection only: a player without a position is not live yet
        const uint32_t owner = static_cast<uint32_t>(entity);
        hub::set(registry, owner, "PLAYER_SESSION_START"_hs, 1.0f);
        raised.push_back(entity);
        log::info("[PlayerHubSessStrtSystem] session start announced (player_owner={} player_id={})",
                  owner, id.player_id);
    }

    // After both walks: the tags that decide membership change only now.
    for (uint32_t i = 0; i < lowered.size(); ++i) {
        registry.emplace_or_replace<PlayerSessDoneTag>(lowered[i]);
    }
    for (uint32_t i = 0; i < raised.size(); ++i) {
        registry.emplace_or_replace<PlayerSessStrtTag>(raised[i]);
    }
}

void PlayerHubSessStrtSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubSessStrtSystem] Stopped");
}

}  // namespace ase::player
