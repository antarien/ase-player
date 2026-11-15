/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_turn_sys.cpp
 * @brief       PlayerHubTurnSystem - lays out the turn rows of an embedding host as turn inputs
 * @description Takes every row HUB_PLR_TURN_REQ, lays it out as PlayerInpTurnComponent on a request entity
 *              of its own and releases the row whatever its outcome.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101554 - receive player input
 * @actor       Player Input Warden (PLAYINPWAR)
 * @design      DSGN_021
 * @category    hub
 * @schedule    Reception
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
 *   [ase-pl-avatar AvatarHostTurnSystem - one row HUB_PLR_TURN_REQ per turn request of the host]
 *          │
 *          │ the player turns his figure without walking
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubTurnSystem           │
 *   │                                             │
 *   │  READS:                                     │
 *   │    → rows HUB_PLR_TURN_REQ / HUB_PLR_TURN   │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    → PlayerInpTurnComponent on a request    │
 *   │      entity, one per row                    │
 *   │    → releases every row it read             │
 *   └─────────────────────────────────────────────┘
 *          │
 *          ▼
 *   PlayerCtrlTurnSystem (Dynamics) adds the yaw onto the open course and turns PlayerStaYawComponent
 *
 * ONE ENTITY PER ROW. Two rows of one tick are two turns; laid out each on its own request entity, none
 * overwrites the other, and the calculation that joins them stays in PlayerCtrlTurnSystem.
 *
 * HUB Pattern (ARCH_ASE_HUB_API, Typisierte Zeilen):
 *
 * READS (from Hub):
 *   row HUB_PLR_TURN_REQ → HUB_PLR_TURN_FIGURE (u32), HUB_PLR_TURN_YAW (f32),
 *                          read with hub::try_get_u32 / hub::try_get_f32
 *
 * WRITES (to Hub):
 *   hub::row_release of every row read, whatever its outcome
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
 * [ ] Sync System: Hub I/O ALLOWED (this IS the I/O boundary)
 */

// INCLUDES - ONLY THESE ARE ALLOWED!
// FORBIDDEN: <vector>, <map>, <unordered_map>, <optional>, <algorithm>
// ALLOWED:   <cstdint>, <cmath>, <cassert>, ase-* headers

// Own header FIRST
#include <ase/player/systems/hub/player_hub_turn_sys.hpp>
// Components from same module
#include <ase/player/components/input/player_inp_turn_comp.hpp>
#include <ase/player/types.hpp>
// Hub for the row API
#include <ase/hub/api.hpp>
// Sentinels
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

// No helper functions needed - one input per row

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerHubTurnSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubTurnSystem] Started");
}

void PlayerHubTurnSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    /**
     * STEP 1: Every turn row of the host - read, checked, laid out as one input, released whatever its
     * outcome
     */
    auto take = [&registry](ecs::Entity row) {
        const uint32_t row_id = static_cast<uint32_t>(row);
        uint32_t named = ase::types::InvalidEntityId;
        float yaw = 0.0f;
        if (!hub::try_get_u32(registry, row_id, "HUB_PLR_TURN_FIGURE"_hs, named) ||
            !hub::try_get_f32(registry, row_id, "HUB_PLR_TURN_YAW"_hs, yaw)) {
            log::warn(log::WRN::CAT::INPUT_TRUNCATED, "PlayerHubTurnSystem", row_id, "HUB_PLR_TURN_REQ",
                      "a turn row without its figure or its yaw turns nothing");
            hub::row_release(registry, row);
            return;
        }
        if (!ase::types::is_val_float(yaw)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerHubTurnSystem", named, "HUB_PLR_TURN_YAW", yaw);
            hub::row_release(registry, row);
            return;
        }
        const auto figure = static_cast<ecs::Entity>(named);
        if (!registry.valid(figure)) {
            log::error(log::ERR::CAT::INVALID_ENTITY, "PlayerHubTurnSystem", named);
            hub::row_release(registry, row);
            return;
        }

        /**
         * STEP 2: The turn as an input of its own
         */
        const ecs::Entity request = registry.create();
        auto& input = registry.emplace<PlayerInpTurnComponent>(request);
        input.figure = figure;
        input.yaw = yaw;
        log::debug("[PlayerHubTurnSystem] turn input filed (figure={} yaw={})", named, yaw);
        hub::row_release(registry, row);
    };
    hub::row_each(registry, "HUB_PLR_TURN_REQ"_hs, "HUB_PLR_TURN"_hs, take);
}

void PlayerHubTurnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubTurnSystem] Stopped");
}

}  // namespace ase::player
