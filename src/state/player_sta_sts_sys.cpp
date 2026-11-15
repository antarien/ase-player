/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sta_sts_sys.cpp
 * @brief       PlayerStaStsSystem - Determine the medium of the player figure from ground contact and velocity
 * @description SHARED System: reads velocity and physics components (no Hub access).
 *              Calculation systems read from Components, not Hub (SYN Pattern).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state/lifecycle/activity
 * @schedule    Dynamics
 * @parity      shared
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-01-22
 * @modified    2026-11-14
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (CAUSA_PLR_STS: Player State Determination)
 *
 *   [PlayerStaVelComponent + PlayerStaPhysComponent]
 *          │
 *          │ velocity and physics data
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerStaStsSystem            │
 *   │  (SHARED - no Hub access)                   │
 *   │                                             │
 *   │  READS (from Components):                   │
 *   │    - PlayerStaVelComponent (vy)             │
 *   │    - PlayerStaPhysComponent (on_ground)     │
 *   │    - PlayerInpWatrComponent (depth_m)       │
 *   │                                             │
 *   │  WRITES (to Components):                    │
 *   │    - PlayerStaStsComponent (sts)            │
 *   │    - PlayerStndWatrTag (laid, taken off)    │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ medium updated: ground, jumping, falling, water
 *          ▼
 *   PlayerStaChnkSystem (chunk association), PlayerLogObsvSystem (figures in water)
 *
 * WATER UNDER THE FIGURE (CAUSA_ASE_HYDRO, "Wasser je Ort", item 6; B0358). PlayerHubWatrSyncSystem lays
 * the depth of the cell under the figure on it in Reception; this system lays PlayerStndWatrTag while that
 * depth lies above PLAYER_WATR_DRY_M and takes it off when the depth falls to it or the measurement is
 * withdrawn. Two passes, neither of which filters on the tag it lays or takes.
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   (none - uses SYN pattern, reads components only)
 *
 * WRITES (to Hub for other modules):
 *   (none - uses SYN pattern, writes to Components only)
 *
 * THE STATE NAMES THE MEDIUM, THE GAIT IS A TAG (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.5). Standing,
 * walking and running used to be three values of sts, chosen here from the sprint input. They are
 * the tags PlayerGaitStndTag, PlayerGaitWalkTag and PlayerGaitRunTag now, set by
 * PlayerStaGaitSystem from the horizontal speed; a figure on the ground gets PLAYER_STATE_GROUND
 * here whatever its gait. The sprint input keeps its reader in PlayerCtrlMovSystem, which picks the
 * speed gear from it.
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
#include <ase/player/systems/state/player_sta_sts_sys.hpp>
// Components from same module
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_phys_comp.hpp>
#include <ase/player/components/state/player_sta_sts_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/input/player_inp_watr_comp.hpp>
#include <ase/player/components/tag/player_stnd_watr_tag.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Logging
#include <ase/log/log.hpp>

namespace ase::player {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * IMPORTANT: Use anonymous namespace, NOT static keyword!
 *   OK: namespace { void helper() {...} }   // CORRECT
 *   NO: static void helper() {...}          // WRONG!
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

// No helper functions needed - all logic inlined in tick()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerStaStsSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerStaStsSystem] Started");
}

void PlayerStaStsSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Create view for player entities with required components (SYN Pattern)
     * The gait on the ground (stand, walk, run) is not decided here: PlayerStaGaitSystem sets it
     * as a tag from the horizontal speed. This system names the medium only.
     */
    auto view = registry.view<
        PlayerStaIdntComponent,
        PlayerStaVelComponent,
        PlayerStaPhysComponent,
        PlayerStaStsComponent
    >();

    for (auto [entity, id, vel, physics, state] : view.each()) {
        (void)id;
        (void)entity;

        /**
         * STEP 2: Determine the medium from ground contact and vertical velocity
         */
        if (physics.on_ground) {
            state.sts = PLAYER_STATE_GROUND;
        } else {
            if (vel.vy > 0.0f) {
                state.sts = PLAYER_STATE_JUMPING;
            } else {
                state.sts = PLAYER_STATE_FALLING;
            }
        }
    }

    /**
     * STEP 3: Water under the figure. The view walks the measured depth and never filters on the tag it
     * lays, so laying or taking PlayerStndWatrTag leaves the set this pass walks unchanged. A cell above
     * PLAYER_WATR_DRY_M carries water, a cell at or below it is dry; taking a tag the figure does not
     * carry is a no-op. PlayerLogObsvSystem reports how many figures stand in water.
     */
    for (auto [entity, water] : registry.view<PlayerInpWatrComponent>().each()) {
        if (water.depth_m > PLAYER_WATR_DRY_M) {
            registry.emplace_or_replace<PlayerStndWatrTag>(entity);
        } else {
            registry.remove<PlayerStndWatrTag>(entity);
        }
    }

    /**
     * STEP 4: A figure whose depth is no longer measured keeps no water. The view walks the figures
     * without a measured depth, again without filtering on the tag it takes.
     */
    for (auto [entity, id] : registry.view<PlayerStaIdntComponent>(entt::exclude<PlayerInpWatrComponent>).each()) {
        (void)id;
        registry.remove<PlayerStndWatrTag>(entity);
    }
}

void PlayerStaStsSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerStaStsSystem] Stopped");
}

}  // namespace ase::player
