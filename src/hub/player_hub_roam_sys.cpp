/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_roam_sys.cpp
 * @brief       PlayerHubRoamSystem - publishes a walker's errand as ordinary player input
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    hub
 * @schedule    Dynamics
 * @created     2026-08-09
 * @modified    2026-08-09
 * @version     1.0.0
 *
 * CAUSAL CHAIN (an errand becomes input, and from there is indistinguishable from a human's)
 *
 *   [PlayerSimWandSystem and PlayerSimRestSystem decided the phase and the course of this tick]
 *          │
 *          │ magnitude, course and gear - all three already stated, none of them computed here
 *          ▼
 *   ┌──────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubRoamSystem (tick)                     │
 *   │                                                              │
 *   │  READS:                                                      │
 *   │    - PlayerStaRoamComponent (magnitude and course)           │
 *   │    - PlayerRoamWandTag / PlayerRoamRestTag (the phase)       │
 *   │    - PlayerRoamRunTag (the gear)                             │
 *   │                                                              │
 *   │  WRITES:                                                     │
 *   │    - Hub PLR_CAM_YAW, PLR_INP_FWD, PLR_INP_SPRINT            │
 *   └──────────────────────────────────────────────────────────────┘
 *          │
 *          │ the very keys a human client's input arrives on
 *          ▼
 *   [PlayerSyncInpSystem → PlayerInpExtComponent → PlayerCtrlMovSystem → velocity → position →
 *    PlayerHubPosSystem → the terrain observer seam → the cell report → the lattice]
 *
 * WHY EVERY BRANCH HERE IS A VIEW
 *
 * Publishing zero while a walker rests, or a sprint bit while it runs, are not decisions - they
 * are the two phases and the two gears, and both already stand as tags. Asking a component for
 * them inside one loop would put a state machine back into an I/O system; four tag-filtered views
 * state the same thing without a single comparison.
 *
 * HUB Pattern (ARCH_ASE_HUB_API v2.0)
 *
 * READS: None
 *
 * WRITES:
 *   PLR_CAM_YAW     (owner = player entity) - the course of the current leg, radians
 *   PLR_INP_FWD     (owner = player entity) - forward magnitude, the errand's while walking, 0 resting
 *   PLR_INP_SPRINT  (owner = player entity) - the gear the magnitude is meant against
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
#include <ase/player/systems/hub/player_hub_roam_sys.hpp>
// The errand row and the phase and gear tags it is published against
#include <ase/player/components/state/player_sta_roam_comp.hpp>
#include <ase/player/components/tag/player_roam_wand_tag.hpp>
#include <ase/player/components/tag/player_roam_rest_tag.hpp>
#include <ase/player/components/tag/player_roam_run_tag.hpp>
// Module constants (the still and the engaged reading of an input key)
#include <ase/player/types.hpp>
// Hub for O(1) API
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

namespace ase::player {

using namespace entt::literals;

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO Registry/View access! Only pure math computation!
 */
namespace {

// No helper needed - every value published here already stands on a component or on a tag.

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerHubRoamSystem::on_start(ecs::Registry& registry) {
    log::debug("[PlayerHubRoamSystem] Started");
    (void)registry;
}

void PlayerHubRoamSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    /**
     * COURSE - published for every walker, in both phases. A resting walker keeps facing where it
     * will set off from, which is what makes a pause look like a pause and not like a reset.
     */
    for (auto [entity, roam] : registry.view<PlayerStaRoamComponent>().each()) {
        hub::set(registry, static_cast<uint32_t>(entity), "PLR_CAM_YAW"_hs, roam.heading);
    }

    /**
     * MAGNITUDE - the errand's own while a leg is walked, and nothing at all while it rests.
     */
    for (auto [entity, roam] :
         registry.view<PlayerStaRoamComponent, PlayerRoamWandTag>().each()) {
        hub::set(registry, static_cast<uint32_t>(entity), "PLR_INP_FWD"_hs, roam.forward);
    }
    for (auto [entity, roam] :
         registry.view<PlayerStaRoamComponent, PlayerRoamRestTag>().each()) {
        (void)roam;
        hub::set(registry, static_cast<uint32_t>(entity), "PLR_INP_FWD"_hs, PLR_ROAM_INPUT_STILL);
    }

    /**
     * GEAR - the two views state which of the two speeds the magnitude is meant against. The
     * excluding view is what keeps the walking gear from needing a tag of its own.
     */
    for (auto [entity, roam] :
         registry.view<PlayerStaRoamComponent, PlayerRoamRunTag>().each()) {
        (void)roam;
        hub::set(registry, static_cast<uint32_t>(entity), "PLR_INP_SPRINT"_hs,
                 PLR_ROAM_INPUT_ENGAGED);
    }
    for (auto [entity, roam] :
         registry.view<PlayerStaRoamComponent>(entt::exclude<PlayerRoamRunTag>).each()) {
        (void)roam;
        hub::set(registry, static_cast<uint32_t>(entity), "PLR_INP_SPRINT"_hs,
                 PLR_ROAM_INPUT_STILL);
    }
}

void PlayerHubRoamSystem::on_stop(ecs::Registry& registry) {
    log::debug("[PlayerHubRoamSystem] Stopped");
    (void)registry;
}

}  // namespace ase::player
