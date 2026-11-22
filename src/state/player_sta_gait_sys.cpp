/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sta_gait_sys.cpp
 * @brief       PlayerStaGaitSystem - set the gait tag of every player figure from its horizontal speed
 * @description SHARED System: reads PlayerStaVelComponent (no Hub access) and moves each figure
 *              onto exactly one of the gait tags PlayerGaitStndTag, PlayerGaitWalkTag,
 *              PlayerGaitRunTag. Standing, walking and running are tags, not values of
 *              PlayerStaStsComponent, so the clip mixer of the figure and every counter read the
 *              gait from a tag-filtered view (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.5).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state
 * @schedule    Dynamics
 * @parity      shared
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-13
 * @modified    2026-11-21
 * @version     00.01.46.00096 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (CAUSA_PLR_GAIT: Gait from the speed)
 *
 *   [PlayerStaVelComponent]  ← PlayerSimStndSystem (vx, vy, vz: the speed ase-physics granted), then
 *                              PlayerCtrlMovSystem (vx, vz: part of the rest to the wish), this tick
 *          │
 *          │ horizontal speed |(vx, vz)|
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerStaGaitSystem           │
 *   │  (SHARED - no Hub access)                   │
 *   │                                             │
 *   │  READS (from Components):                   │
 *   │    - PlayerStaIdntComponent (player row)    │
 *   │    - PlayerStaVelComponent (vx, vz)         │
 *   │                                             │
 *   │  WRITES (Tags):                             │
 *   │    - PlayerGaitStndTag  below stand max     │
 *   │    - PlayerGaitWalkTag  stand to walk max   │
 *   │    - PlayerGaitRunTag   above walk max      │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ exactly one gait tag per figure
 *          ▼
 *   PlayerLogObsvSystem (gait counters), the hub sync of ase-pl-avatar (clip mix in the adapter)
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   (none - SYN pattern, reads the velocity component)
 *
 * WRITES (to Hub for other modules):
 *   (none - SYN pattern, writes tags only; ase-pl-avatar publishes the gait)
 *
 * ONE VIEW PER GAIT, EACH EXCLUDING ITS OWN TAG. A figure that already carries the gait its speed
 * asks for is not visited at all, so a standing crowd costs three exclusion checks and no
 * storage write. A figure that changes gait is visited by the one view whose tag it lacks: the
 * other two gait tags are removed and its own is placed. A figure without any gait tag - fresh
 * from the spawn or from a migration - is visited the same way and gets its first one.
 *
 * THE THREE BANDS SHARE NO POINT. The walking band is closed on both ends (types::is_in_rng_f is
 * inclusive), so standing lies strictly below its floor and running strictly above its ceiling. A
 * speed exactly on a bound therefore belongs to one band only and cannot flip a figure between
 * two tags tick after tick.
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
#include <ase/player/systems/state/player_sta_gait_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/tag/player_gait_stnd_tag.hpp>
#include <ase/player/components/tag/player_gait_walk_tag.hpp>
#include <ase/player/components/tag/player_gait_run_tag.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Range predicate for the walking band (is_in_rng_f, inclusive)
#include <ase/types/types.hpp>
// Logging
#include <ase/log/log.hpp>

namespace ase::player {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * Pure math only - no View, no Registry.
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

/// Squared horizontal speed of a velocity; the bounds are squared at the call, so no root is drawn.
float speed_xz_sq(float vx, float vz) {
    return vx * vx + vz * vz;
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerStaGaitSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerStaGaitSystem] Started");
}

void PlayerStaGaitSystem::tick(ecs::Registry& registry, float /*dt*/) {
    const float stnd_max_sq = PLAYER_GAIT_STND_SPEED_MAX * PLAYER_GAIT_STND_SPEED_MAX;
    const float walk_max_sq = PLAYER_GAIT_WALK_SPEED_MAX * PLAYER_GAIT_WALK_SPEED_MAX;

    /**
     * THE VIEW NEVER FILTERS ON A GAIT MARKER. Each step lays a marker the walk would otherwise
     * filter on (an exclude of the marker it sets), and that changes the iteration set from
     * inside the loop. The figures are walked over identity and velocity alone; the speed band
     * decides, and emplace_or_replace lays the marker whether the figure held it already or not.
     */
    auto figures = registry.view<PlayerStaIdntComponent, PlayerStaVelComponent>();

    /**
     * STEP 1: figures whose speed has fallen below the walking floor stand
     */
    for (auto [entity, id, vel] : figures.each()) {
        (void)id;
        if (!(speed_xz_sq(vel.vx, vel.vz) < stnd_max_sq)) {
            continue;
        }
        registry.remove<PlayerGaitWalkTag, PlayerGaitRunTag>(entity);
        registry.emplace_or_replace<PlayerGaitStndTag>(entity);
    }

    /**
     * STEP 2: figures whose speed lies in the walking band walk
     */
    for (auto [entity, id, vel] : figures.each()) {
        (void)id;
        const float speed_sq = speed_xz_sq(vel.vx, vel.vz);
        if (!types::is_in_rng_f(speed_sq, stnd_max_sq, walk_max_sq)) {
            continue;
        }
        registry.remove<PlayerGaitStndTag, PlayerGaitRunTag>(entity);
        registry.emplace_or_replace<PlayerGaitWalkTag>(entity);
    }

    /**
     * STEP 3: figures whose speed lies above the walking ceiling run
     */
    for (auto [entity, id, vel] : figures.each()) {
        (void)id;
        if (!(speed_xz_sq(vel.vx, vel.vz) > walk_max_sq)) {
            continue;
        }
        registry.remove<PlayerGaitStndTag, PlayerGaitWalkTag>(entity);
        registry.emplace_or_replace<PlayerGaitRunTag>(entity);
    }
}

void PlayerStaGaitSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerStaGaitSystem] Stopped");
}

}  // namespace ase::player
