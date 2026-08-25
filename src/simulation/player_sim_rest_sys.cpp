/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sim_rest_sys.cpp
 * @brief       PlayerSimRestSystem - rests a backend-driven player and decides its next heading
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-08-09
 * @modified    2026-08-09
 * @version     1.0.0
 *
 * CAUSAL CHAIN (the pause between two legs, and the decision that ends it)
 *
 *   [PlayerLifeSpwnSystem starts every walker here, and PlayerSimWandSystem returns it here at
 *    the end of every leg with the rest length already drawn]
 *          │
 *          │ the seconds the walker still stands
 *          ▼
 *   ┌──────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSimRestSystem (tick)                     │
 *   │                                                              │
 *   │  READS:                                                      │
 *   │    - PlayerStaRoamComponent (the rest clock, the old course) │
 *   │    - PlayerRoamRestTag (the resting phase)                   │
 *   │                                                              │
 *   │  WRITES:                                                     │
 *   │    - PlayerStaRoamComponent (leg counter, new heading, clock)│
 *   │    - PlayerRoamWandTag (handover at the end of the rest)     │
 *   │    - PlayerRoamRestTag (removed on the same handover)        │
 *   └──────────────────────────────────────────────────────────────┘
 *          │
 *          │ a NEW course, never the old one continued
 *          ▼
 *   [PlayerSimWandSystem walks the leg; PlayerHubRoamSystem publishes the course as PLR_CAM_YAW]
 *
 * WHY THE COURSE CHANGE HAS A FLOOR
 *
 * The turn drawn here is bounded away from zero at both ends of its span. A draw that could come
 * out at nothing would let a walker leave a rest on the very course it arrived on, and a journey
 * made of such legs is a straight line with pauses in it - which is the shape this whole phase
 * pair exists to avoid. The floor is a property of a course CHANGE, not a tuning factor.
 *
 * WHY THE PAUSE IS NOT DECORATION
 *
 * The cell report downstream is dwell-gated: ase-terrain settles a crossing only after a session
 * has stood past the hysteresis band of the freshly entered cell for a while. A walker who never
 * lingers reports nothing at all. The pause is therefore what makes a trail exist in the first
 * place, and what lets a dwelt-in place read as a denser mark than a place merely passed through.
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
#include <ase/player/systems/simulation/player_sim_rest_sys.hpp>
// The errand row and the two phase tags of a journey
#include <ase/player/components/state/player_sta_roam_comp.hpp>
#include <ase/player/components/tag/player_roam_rest_tag.hpp>
#include <ase/player/components/tag/player_roam_wand_tag.hpp>
// Module constants (leg and rest spans, turn span, draw mixers, deferred batch bound)
#include <ase/player/types.hpp>
// Math (the full turn from L0 - no literal 2*pi here)
#include <ase/math/math.hpp>
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

// The draw sequence itself is ONE statement in types.hpp (plr_roam_fraction, plr_roam_span) -
// three systems draw from it, and a copy per system would be three sequences.

/**
 * The course after a turn, brought back into one full turn.
 *
 * The turn is signed and bounded by the span, so ONE conditional correction per end is enough -
 * a modulo would spend a division on a case that cannot arise.
 */
float roam_heading_after(float heading, float turn, float side) {
    const float turned = heading + side * turn;
    const float lifted = (turned < 0.0f) ? (turned + math::TWO_PI) : turned;
    return (lifted >= math::TWO_PI) ? (lifted - math::TWO_PI) : lifted;
}

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerSimRestSystem::on_start(ecs::Registry& registry) {
    log::debug("[PlayerSimRestSystem] Started");
    (void)registry;
}

void PlayerSimRestSystem::tick(ecs::Registry& registry, float dt) {
    // Pass-local buffer for the phase handover, applied after the View closes.
    ecs::Entity ready[PLR_ROAM_DEFER_BATCH_MAX] = {};
    uint32_t ready_count = 0;

    /**
     * REST PASS
     * One walk over the walkers that stand. Nothing but the clock moves - the forward magnitude
     * is not published at all while this phase holds, so a rest needs no zero written anywhere.
     */
    {
        auto view = registry.view<PlayerStaRoamComponent, PlayerRoamRestTag>();
        for (auto [entity, roam] : view.each()) {
            roam.leg_sec -= dt;
            if (roam.leg_sec > 0.0f) {
                continue;
            }
            // Capacity BEFORE the handover, for the same reason the walking phase checks it there:
            // a walker whose departure could not be staged keeps standing one more tick.
            if (ready_count >= PLR_ROAM_DEFER_BATCH_MAX) {
                continue;
            }
            ready[ready_count] = entity;
            ++ready_count;
        }
    }

    /**
     * DEPARTURE
     * The leg counter advances FIRST, so the new leg draws on its own number and no two legs of a
     * journey can come out the same. Course change and leg length then follow from that number on
     * two separate mixers, and the sign of the turn on a third.
     */
    for (uint32_t i = 0; i < ready_count; ++i) {
        auto& roam = registry.get<PlayerStaRoamComponent>(ready[i]);
        const uint32_t owner = static_cast<uint32_t>(ready[i]);
        roam.leg_index += 1u;

        const float turn = plr_roam_span(plr_roam_fraction(owner, roam.leg_index, PLR_ROAM_MIX_TURN),
                                         PLR_ROAM_TURN_MIN_RAD, PLR_ROAM_TURN_MAX_RAD);
        const float side_draw = plr_roam_fraction(owner, roam.leg_index, PLR_ROAM_MIX_SIDE);
        const float side = (side_draw < PLR_ROAM_SIDE_SPLIT) ? -1.0f : 1.0f;
        roam.heading = roam_heading_after(roam.heading, turn, side);
        roam.leg_sec = plr_roam_span(plr_roam_fraction(owner, roam.leg_index, PLR_ROAM_MIX_LEG),
                                     PLR_ROAM_LEG_MIN_SEC, PLR_ROAM_LEG_MAX_SEC);

        registry.remove<PlayerRoamRestTag>(ready[i]);
        registry.emplace<PlayerRoamWandTag>(ready[i]);
    }

    if (ready_count > 0) {
        log::debug("[PlayerSimRestSystem] {} walker(s) rested out and set off on a new course",
                   ready_count);
    }
}

void PlayerSimRestSystem::on_stop(ecs::Registry& registry) {
    log::debug("[PlayerSimRestSystem] Stopped");
    (void)registry;
}

}  // namespace ase::player
