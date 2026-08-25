/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sim_wand_sys.cpp
 * @brief       PlayerSimWandSystem - walks the current leg of a backend-driven player's journey
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-08-09
 * @modified    2026-08-09
 * @version     1.0.0
 *
 * CAUSAL CHAIN (one leg of a journey)
 *
 *   [PlayerLifeSpwnSystem gave the walker its errand and started it in the resting phase;
 *    PlayerSimRestSystem decided this leg's heading and handed the walker over]
 *          │
 *          │ the seconds this heading is still held
 *          ▼
 *   ┌──────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSimWandSystem (tick)                     │
 *   │                                                              │
 *   │  READS:                                                      │
 *   │    - PlayerStaRoamComponent (the leg clock)                  │
 *   │    - PlayerRoamWandTag (the walking phase)                   │
 *   │                                                              │
 *   │  WRITES:                                                     │
 *   │    - PlayerStaRoamComponent (leg clock, next rest length)    │
 *   │    - PlayerRoamRestTag (handover at the end of the leg)      │
 *   │    - PlayerRoamWandTag (removed on the same handover)        │
 *   └──────────────────────────────────────────────────────────────┘
 *          │
 *          │ the magnitude itself is untouched - it belongs to the errand, not to the tick
 *          ▼
 *   [PlayerHubRoamSystem publishes it as PLR_INP_FWD while the walking tag stands, and zero once
 *    the resting tag does - so a pause is a pause without anyone writing a zero into the row]
 *
 * WHY THIS SYSTEM WRITES NEITHER VELOCITY NOR POSITION
 *
 * A walker is a REAL ase-player entity, and the only thing that separates it from a human client
 * is where its input comes from. Writing velocity here would make it a second kind of actor whose
 * motion nobody else could account for - which is exactly what the cheat lever next door does ON
 * PURPOSE, and exactly what an emergence proof may not do. The input travels the ordinary seam
 * and PlayerCtrlMovSystem turns it into motion under the ordinary authority.
 *
 * HUB Pattern (ARCH_ASE_HUB_API v2.0)
 *
 * READS: None (the errand is a component, and the phase is a tag beside it)
 *
 * WRITES: None (the hub half of the journey is PlayerHubRoamSystem - SYN keeps I/O out of here)
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
 * [ ] Safe deletion (first collect, then delete)?
 * [ ] Not deleting other entities during iteration?
 * [ ] Not invalidating references during iteration?
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
 * [ ] NO ARRAYS! (use Entity-per-Item + Tags!)
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
 * [ ] NO switch/case statements? (use Tag-filtered Views or lookup tables!)
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
// FORBIDDEN: <vector>, <map>, <unordered_map>, <optional>, <algorithm>
// ALLOWED:   <cstdint>, <cmath>, <cassert>, ase-* headers

// Own header FIRST
#include <ase/player/systems/simulation/player_sim_wand_sys.hpp>
// The errand row and the two phase tags of a journey
#include <ase/player/components/state/player_sta_roam_comp.hpp>
#include <ase/player/components/tag/player_roam_wand_tag.hpp>
#include <ase/player/components/tag/player_roam_rest_tag.hpp>
// Module constants (leg and rest spans, draw mixers, deferred batch bound)
#include <ase/player/types.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

#include <entt/core/hashed_string.hpp>

namespace ase::player {

using namespace entt::literals;

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO Registry/View access! Only pure math computation!
 */
namespace {

// No helper needed - the draw sequence of a walker is ONE statement in types.hpp
// (plr_roam_fraction, plr_roam_span), because three systems draw from it and a copy per system
// would be three sequences the day one of them is touched.

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerSimWandSystem::on_start(ecs::Registry& registry) {
    log::debug("[PlayerSimWandSystem] Started");
    (void)registry;
}

void PlayerSimWandSystem::tick(ecs::Registry& registry, float dt) {
    // Pass-local buffer for the phase handover, applied after the View closes.
    ecs::Entity ended[PLR_ROAM_DEFER_BATCH_MAX] = {};
    uint32_t ended_count = 0;

    /**
     * WALK PASS
     * One walk over the walkers that are on a leg. The clock is the whole of it: the magnitude
     * and the heading belong to the errand and were decided before this leg began, so a tick of
     * walking changes exactly one number.
     */
    {
        auto view = registry.view<PlayerStaRoamComponent, PlayerRoamWandTag>();
        for (auto [entity, roam] : view.each()) {
            roam.leg_sec -= dt;
            if (roam.leg_sec > 0.0f) {
                continue;
            }
            // Capacity is checked BEFORE the handover, never after: a leg whose end could not be
            // staged keeps its tag and ends in the next pass, which costs one tick and loses
            // nothing. Clearing the tag first and failing to stage would leave a walker with no
            // phase at all - a state no view would ever pick up again.
            if (ended_count >= PLR_ROAM_DEFER_BATCH_MAX) {
                continue;
            }
            ended[ended_count] = entity;
            ++ended_count;
        }
    }

    /**
     * HANDOVER
     * The rest length is drawn on the leg counter of the leg that just ended, on its own mixer -
     * so the pause of a journey is as varied as its legs and independent of them.
     */
    for (uint32_t i = 0; i < ended_count; ++i) {
        auto& roam = registry.get<PlayerStaRoamComponent>(ended[i]);
        const uint32_t owner = static_cast<uint32_t>(ended[i]);
        roam.leg_sec = plr_roam_span(plr_roam_fraction(owner, roam.leg_index, PLR_ROAM_MIX_REST),
                                     PLR_ROAM_REST_MIN_SEC, PLR_ROAM_REST_MAX_SEC);
        registry.remove<PlayerRoamWandTag>(ended[i]);
        registry.emplace<PlayerRoamRestTag>(ended[i]);
    }

    if (ended_count > 0) {
        log::debug("[PlayerSimWandSystem] {} walker(s) reached the end of a leg and rest",
                   ended_count);
    }
}

void PlayerSimWandSystem::on_stop(ecs::Registry& registry) {
    log::debug("[PlayerSimWandSystem] Stopped");
    (void)registry;
}

}  // namespace ase::player
