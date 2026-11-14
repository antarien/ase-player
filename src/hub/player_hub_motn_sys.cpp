/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_motn_sys.cpp
 * @brief       PlayerHubMotnSystem - publishes course, horizontal velocity and gait of every figure as a HUB_PLR_MOTN row
 * @description Adopts the hub row kind HUB_PLR_MOTN onto every entity that carries PlayerSpndTag, fills its
 *              three fields and states the gait of the figure as one of the three marks of the kind.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101611 - animate character motion
 * @actor       SS/PC
 * @category    hub
 * @schedule    Dynamics
 * @created     2026-11-13
 * @modified    2026-11-13
 * @version     1.0.0
 *
 * CAUSAL CHAIN (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.5 and 07.8: the drawing host blends the clips of the
 * figure by its gait and turns it along its course)
 *
 *   [PlayerCtrlMovSystem steers PlayerStaVelComponent and turns PlayerStaYawComponent, PlayerSimStndSystem
 *    takes vy from ase-physics;
 *    PlayerStaGaitSystem sets exactly one of PlayerGaitStndTag, PlayerGaitWalkTag, PlayerGaitRunTag]
 *          │
 *          ▼
 *   ┌─────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubMotnSystem (Dynamics)    │
 *   │                                                 │
 *   │  READS:                                         │
 *   │    → PlayerSpndTag (the figures)                │
 *   │    → PlayerStaYawComponent, PlayerStaVelComponent│
 *   │    → PlayerGaitStndTag, PlayerGaitWalkTag,      │
 *   │      PlayerGaitRunTag                           │
 *   │                                                 │
 *   │  WRITES:                                        │
 *   │    → hub row kind HUB_PLR_MOTN (adopt), fields  │
 *   │      HUB_PLR_MOTN_YAW, _VEL_X, _VEL_Y, one mark │
 *   │      HUB_PLR_GAIT_STND, _WALK or _RUN           │
 *   └─────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   [AvatarHostFigExptSystem of ase-pl-avatar walks each mark and files the figure on avatar.figure.v1]
 *
 * WHY A SYSTEM OF ITS OWN. PlayerStaGaitSystem decides the gait from the velocity and
 * PlayerSimStndSystem takes the velocity back; a hub write in either file is hub I/O beside calculation
 * (HUB_IO_MIXED_WITH_MATH, WRFL_ASE_SYN_PATTERN). This system publishes what they settled and computes
 * nothing.
 *
 * EVERY VALUE IS FILLED IN EVERY PASS from its one home: the course from PlayerStaYawComponent, the
 * velocity from PlayerStaVelComponent, the gait from the tag PlayerStaGaitSystem set in this frame. A
 * reader of the row never sees a value older than the frame.
 *
 * THE AXIS ORDER OF THE STAR. This module carries the height in vy and spans the ground with vx and vz;
 * the star orders the first horizontal axis, the second horizontal axis, the height (PLR_POS_X/Y/Z,
 * PlayerSimWrldSystem). HUB_PLR_MOTN_VEL_X is vx and HUB_PLR_MOTN_VEL_Y is vz, so a reader pairs the
 * velocity with PLR_POS_X and PLR_POS_Y without a swap of its own.
 *
 * ONE MARK PER FIGURE. Each gait has its own view; the view tags its mark and untags the two others in the
 * same step, so a figure whose gait changed carries the new mark only. A figure without a gait tag (a body
 * PlayerStaGaitSystem has not reached yet) carries no mark and is not drawn as walking.
 *
 * THE ROW ENDS WITH THE FIGURE. Nobody withdraws it; when the figure entity is destroyed, the storage of
 * the kind drops it with it.
 *
 * HUB Pattern (ARCH_ASE_HUB_API, Typisierte Zeilen):
 *
 * READS (from player module via Components):
 *   PlayerSpndTag, PlayerStaYawComponent, PlayerStaVelComponent, PlayerGait*Tag
 *
 * WRITES (to Hub for other modules):
 *   row kind HUB_PLR_MOTN → hub::row_adopt onto the figure entity, idempotent
 *   fields HUB_PLR_MOTN_YAW, HUB_PLR_MOTN_VEL_X, HUB_PLR_MOTN_VEL_Y → hub::set_f64
 *   marks HUB_PLR_GAIT_STND, HUB_PLR_GAIT_WALK, HUB_PLR_GAIT_RUN → hub::row_tag, hub::row_untag
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
// FORBIDDEN: <vector>, <map>, <unordered_map>, <optional>, <algorithm>
// ALLOWED:   <cstdint>, <cmath>, <cassert>, ase-* headers

// Own header FIRST
#include <ase/player/systems/hub/player_hub_motn_sys.hpp>
// Components from same module
#include <ase/player/components/tag/player_spnd_tag.hpp>
#include <ase/player/components/tag/player_gait_stnd_tag.hpp>
#include <ase/player/components/tag/player_gait_walk_tag.hpp>
#include <ase/player/components/tag/player_gait_run_tag.hpp>
#include <ase/player/components/state/player_sta_yaw_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
// Hub for the row API
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

namespace ase::player {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * IMPORTANT: Use anonymous namespace, NOT static keyword!
 *   namespace { void helper() {...} }   // CORRECT
 *   static void helper() {...}          // WRONG!
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

// No helper functions needed - every value is filled inline, mark by mark

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerHubMotnSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubMotnSystem] Started");
}

void PlayerHubMotnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Every figure is a row of the kind, and its course and horizontal velocity are filled in
     * this pass. hub::row_adopt is idempotent; the kind lives in a storage of its own, so adopting into
     * it does not change the set this view walks.
     */
    for (auto [figure, yaw, vel] : registry.view<PlayerSpndTag, PlayerStaYawComponent, PlayerStaVelComponent>().each()) {
        hub::row_adopt(registry, figure, "HUB_PLR_MOTN"_hs);
        const auto owner = static_cast<uint32_t>(figure);
        hub::set_f64(registry, owner, "HUB_PLR_MOTN_YAW"_hs, static_cast<double>(yaw.yaw));
        hub::set_f64(registry, owner, "HUB_PLR_MOTN_VEL_X"_hs, static_cast<double>(vel.vx));
        hub::set_f64(registry, owner, "HUB_PLR_MOTN_VEL_Y"_hs, static_cast<double>(vel.vz));
    }

    /**
     * STEP 2: The gait as one mark, one view per gait - the figure standing still
     */
    for (auto figure : registry.view<PlayerSpndTag, PlayerGaitStndTag>()) {
        hub::row_adopt(registry, figure, "HUB_PLR_MOTN"_hs);
        hub::row_untag(registry, figure, "HUB_PLR_GAIT_WALK"_hs);
        hub::row_untag(registry, figure, "HUB_PLR_GAIT_RUN"_hs);
        hub::row_tag(registry, figure, "HUB_PLR_GAIT_STND"_hs);
    }

    /**
     * STEP 3: The figure walking
     */
    for (auto figure : registry.view<PlayerSpndTag, PlayerGaitWalkTag>()) {
        hub::row_adopt(registry, figure, "HUB_PLR_MOTN"_hs);
        hub::row_untag(registry, figure, "HUB_PLR_GAIT_STND"_hs);
        hub::row_untag(registry, figure, "HUB_PLR_GAIT_RUN"_hs);
        hub::row_tag(registry, figure, "HUB_PLR_GAIT_WALK"_hs);
    }

    /**
     * STEP 4: The figure running
     */
    for (auto figure : registry.view<PlayerSpndTag, PlayerGaitRunTag>()) {
        hub::row_adopt(registry, figure, "HUB_PLR_MOTN"_hs);
        hub::row_untag(registry, figure, "HUB_PLR_GAIT_STND"_hs);
        hub::row_untag(registry, figure, "HUB_PLR_GAIT_WALK"_hs);
        hub::row_tag(registry, figure, "HUB_PLR_GAIT_RUN"_hs);
    }
}

void PlayerHubMotnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubMotnSystem] Stopped");
}

}  // namespace ase::player
