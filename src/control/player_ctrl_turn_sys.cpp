/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_ctrl_turn_sys.cpp
 * @brief       PlayerCtrlTurnSystem - turns a standing figure on the spot out of its open course
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @design      DSGN_030
 * @category    action/control
 * @schedule    Dynamics
 * @created     2026-11-14
 * @modified    2026-11-14
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.7: the figure turns where it stands)
 *
 *   [PlayerHubTurnSystem - PlayerInpTurnComponent on a request entity per turn row of ase-pl-avatar]
 *          │
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerCtrlTurnSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    → PlayerInpTurnComponent (the inputs)    │
 *   │    → PlayerReqTurnComponent (open course)   │
 *   │    → PlayerInpExtComponent (does it walk?)  │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    → PlayerReqTurnComponent (added onto,    │
 *   │      turned out of, removed when empty)     │
 *   │    → PlayerStaYawComponent (the heading)    │
 *   │    → PlayerDrtyTag (the heading changed)    │
 *   │    → destroys every input it took           │
 *   └─────────────────────────────────────────────┘
 *          │
 *          ▼
 *   [ase-pl-avatar lays the heading out on avatar.body.v1; the Godot scene turns the body and plays the
 *    turn clip from the gap between the drawn and the carried heading]
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   (none - uses SYN pattern, reads from PlayerInpTurnComponent that PlayerHubTurnSystem laid out)
 *
 * WRITES (to Hub for other modules):
 *   (none - uses SYN pattern, writes to Components only)
 *
 * ONE RATE FOR BOTH TURNS. The figure turns on the spot at MOVEMENT_DEFAULT_TURN_SPEED, the same rate at
 * which PlayerCtrlInpSystem turns it toward the camera while it walks. A course larger than one tick can
 * turn stays open and is turned out over the next ticks; a course that fits is turned out at once.
 *
 * WALKING WINS. A figure that walks takes its heading from the camera; a turn on the spot laid over it
 * would give the heading a second will. Its open course is dropped and reported, never kept for later.
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
#include <ase/player/systems/control/player_ctrl_turn_sys.hpp>
// Components from same module
#include <ase/player/components/input/player_inp_ext_comp.hpp>
#include <ase/player/components/input/player_inp_turn_comp.hpp>
#include <ase/player/components/request/player_req_turn_comp.hpp>
#include <ase/player/components/state/player_sta_yaw_comp.hpp>
#include <ase/player/components/tag/player_drty_tag.hpp>
#include <ase/player/types.hpp>
// Math from Layer 0
#include <ase/math/math.hpp>
// Sentinels and float predicates
#include <ase/types/types.hpp>
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

// No helper functions needed - the turn stays INLINE (visible to validator)

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerCtrlTurnSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerCtrlTurnSystem] Started");
}

void PlayerCtrlTurnSystem::tick(ecs::Registry& registry, float dt) {
    /**
     * STEP 1: Every turn input onto the open course of its figure - the input entity goes whatever its
     * outcome. Iterator pattern: the iterator steps on before the entity it stands on is destroyed.
     */
    auto inputs = registry.view<PlayerInpTurnComponent>();
    auto input_it = inputs.begin();
    const auto input_end = inputs.end();
    while (input_it != input_end) {
        const ecs::Entity request = *input_it;
        ++input_it;
        const auto& input = inputs.get<PlayerInpTurnComponent>(request);
        const ecs::Entity figure = input.figure;
        if (!registry.valid(figure)) {
            log::error(log::ERR::CAT::INVALID_ENTITY, "PlayerCtrlTurnSystem", static_cast<uint32_t>(figure));
            registry.destroy(request);
            continue;
        }
        if (registry.try_get<PlayerStaYawComponent>(figure) == nullptr) {
            log::error(log::ERR::CAT::COMPONENT_MISSING, "PlayerCtrlTurnSystem", static_cast<uint32_t>(figure),
                       "PlayerStaYawComponent");
            registry.destroy(request);
            continue;
        }
        auto& course = registry.get_or_emplace<PlayerReqTurnComponent>(figure);
        course.open_yaw += input.yaw;
        registry.destroy(request);
    }

    /**
     * STEP 2: Every open course turned out by at most one tick of the turn rate. Iterator pattern: the
     * iterator steps on before the course of the figure it stands on is removed.
     */
    const float max_turn = MOVEMENT_DEFAULT_TURN_SPEED * dt;
    auto courses = registry.view<PlayerStaYawComponent, PlayerReqTurnComponent>();
    auto course_it = courses.begin();
    const auto course_end = courses.end();
    while (course_it != course_end) {
        const ecs::Entity figure = *course_it;
        ++course_it;
        auto& yaw = courses.get<PlayerStaYawComponent>(figure);
        auto& course = courses.get<PlayerReqTurnComponent>(figure);

        /**
         * STEP 2a: A walking figure takes its heading from the camera - the open course is dropped
         */
        const auto* walk = registry.try_get<PlayerInpExtComponent>(figure);
        if (walk != nullptr && (!types::is_zero_float(walk->inp_fwd) || !types::is_zero_float(walk->inp_str))) {
            log::debug("[PlayerCtrlTurnSystem] figure {} walks, open course {} rad dropped",
                       static_cast<uint32_t>(figure), course.open_yaw);
            registry.remove<PlayerReqTurnComponent>(figure);
            continue;
        }

        /**
         * STEP 2b: One step of the course, the heading wrapped into [0, 2π)
         */
        const float step = math::clamp(course.open_yaw, -max_turn, max_turn);
        yaw.yaw += step;
        while (yaw.yaw < 0.0f) yaw.yaw += math::TWO_PI;
        while (yaw.yaw >= math::TWO_PI) yaw.yaw -= math::TWO_PI;
        course.open_yaw -= step;
        registry.emplace_or_replace<PlayerDrtyTag>(figure);

        /**
         * STEP 2c: A course turned out is no request any more
         */
        if (types::is_zero_float(course.open_yaw)) {
            registry.remove<PlayerReqTurnComponent>(figure);
        }
    }
}

void PlayerCtrlTurnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerCtrlTurnSystem] Stopped");
}

}  // namespace ase::player
