/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_stnd_sync_sys.cpp
 * @brief       PlayerHubStndSyncSystem - lays the answer of ase-physics on every walking figure
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    input
 * @schedule    Reception
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-14
 * @modified    2026-11-14
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.13, DoD 07-K: ase-physics resolves gravity and ground
 * contact of the figure and answers with HUB_PHYS_STAND)
 *
 *   [PhysicsHubStndPubSystem of ase-physics files place, vertical speed and placement count on the row
 *    HUB_PHYS_STAND of the figure in Collision, the mark HUB_PHYS_STAND_GRND while it stands on its ground]
 *          │
 *          ▼
 *   ┌──────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubStndSyncSystem (Reception)            │
 *   │  READS:  row HUB_PHYS_STAND, its fields and its mark         │
 *   │  WRITES: PlayerInpStndComponent, PlayerInpStndVlcyComponent, │
 *   │          PlayerStndGrndTag                                   │
 *   └──────────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   [PlayerSimStndSystem in Dynamics turns the answer back into the chunk address of the figure]
 *
 * HUB Pattern (ARCH_ASE_HUB_API, Typisierte Zeilen)
 *
 * READS (from Hub, per figure):
 *   row HUB_PHYS_STAND (hub::row_each)
 *   HUB_PHYS_STAND_X, HUB_PHYS_STAND_Y, HUB_PHYS_STAND_Z, HUB_PHYS_STAND_VEL_Y (hub::try_get_f32)
 *   HUB_PHYS_STAND_VEL_X, HUB_PHYS_STAND_VEL_Z (hub::try_get_f32)
 *   HUB_PHYS_STAND_SEQ (hub::try_get_u32)
 *   mark HUB_PHYS_STAND_GRND (hub::row_each with the mark)
 *
 * WRITES (to Components):
 *   PlayerInpStndComponent.x, y, z, vel_y, seq
 *   PlayerInpStndVlcyComponent.vel_x, vel_z (the level speed ase-physics granted, after wall and water)
 *   PlayerStndGrndTag
 *
 * THE MARK IS STATED ANEW IN EVERY PASS. The tag is taken off every figure first and laid again on exactly the
 * figures whose row carries the mark now, so a figure that left its ground in the last tick of ase-physics
 * carries no tag, and nothing older than the answer survives.
 *
 * A WITHDRAWN ANSWER LEAVES NOTHING BEHIND. ase-physics withdraws the row when the figure stops being a body
 * (PhysicsHubWalkSyncSystem); the figure then loses PlayerInpStndComponent in the same pass, so
 * PlayerSimStndSystem never takes over a place that no body stands at. The pass walks the placed figures
 * (PlayerStaPlcmComponent, laid by PlayerSimWalkSystem before any walk is filed), not the answers, so the
 * removal never changes the set the pass walks.
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
#include <ase/player/systems/sync/player_hub_stnd_sync_sys.hpp>
// Components from same module
#include <ase/player/components/input/player_inp_stnd_comp.hpp>
#include <ase/player/components/input/player_inp_stnd_vlcy_comp.hpp>
#include <ase/player/components/state/player_sta_plcm_comp.hpp>
#include <ase/player/components/tag/player_stnd_grnd_tag.hpp>
// Hub for I/O (ALLOWED in Sync Systems!)
#include <ase/hub/api.hpp>
// Types for sentinel and range checks
#include <ase/types/types.hpp>
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

// No helpers - range validation must be INLINE (visible to validator)

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerHubStndSyncSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubStndSyncSystem] Started");
}

void PlayerHubStndSyncSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * PASS 1: every answer of ase-physics. The row lives in a storage of its own, so laying the component on the
     * visited figure does not change the set this pass walks.
     */
    hub::row_each(registry, "HUB_PHYS_STAND"_hs, [&](ecs::Entity figure) {
        const auto row = static_cast<uint32_t>(figure);

        /**
         * STEP 1: Read every field; ase-physics writes all seven together, so a missing one is a defect of the
         * writer, and the figure keeps no answer of an earlier pass.
         */
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float vel_x = 0.0f;
        float vel_y = 0.0f;
        float vel_z = 0.0f;
        uint32_t seq = 0;
        if (!hub::try_get_f32(registry, row, "HUB_PHYS_STAND_X"_hs, x) ||
            !hub::try_get_f32(registry, row, "HUB_PHYS_STAND_Y"_hs, y) ||
            !hub::try_get_f32(registry, row, "HUB_PHYS_STAND_Z"_hs, z) ||
            !hub::try_get_f32(registry, row, "HUB_PHYS_STAND_VEL_X"_hs, vel_x) ||
            !hub::try_get_f32(registry, row, "HUB_PHYS_STAND_VEL_Y"_hs, vel_y) ||
            !hub::try_get_f32(registry, row, "HUB_PHYS_STAND_VEL_Z"_hs, vel_z) ||
            !hub::try_get_u32(registry, row, "HUB_PHYS_STAND_SEQ"_hs, seq)) {
            log::error(log::ERR::CAT::HUB_NOT_FOUND, "PlayerHubStndSyncSystem", row, "HUB_PHYS_STAND_X");
            registry.remove<PlayerInpStndComponent, PlayerInpStndVlcyComponent>(figure);
            return;
        }

        /**
         * STEP 2: A place that is no number is taken for no place.
         */
        if (!types::is_val_float(x) || !types::is_val_float(y) || !types::is_val_float(z)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerHubStndSyncSystem", row, "HUB_PHYS_STAND_Y", y);
            registry.remove<PlayerInpStndComponent, PlayerInpStndVlcyComponent>(figure);
            return;
        }
        if (!types::is_val_float(vel_y)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerHubStndSyncSystem", row, "HUB_PHYS_STAND_VEL_Y", vel_y);
            registry.remove<PlayerInpStndComponent, PlayerInpStndVlcyComponent>(figure);
            return;
        }
        if (!types::is_val_float(vel_x) || !types::is_val_float(vel_z)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerHubStndSyncSystem", row, "HUB_PHYS_STAND_VEL_X", vel_x);
            registry.remove<PlayerInpStndComponent, PlayerInpStndVlcyComponent>(figure);
            return;
        }

        /**
         * STEP 3: Lay the answer on the figure; the first one is named with its placement.
         */
        if (!registry.all_of<PlayerInpStndComponent>(figure)) {
            log::debug("[PlayerHubStndSyncSystem] figure {} walks as a body of ase-physics, placement {}", row, seq);
        }
        auto& stand = registry.get_or_emplace<PlayerInpStndComponent>(figure);
        stand.x = x;
        stand.y = y;
        stand.z = z;
        stand.vel_y = vel_y;
        stand.seq = seq;
        auto& granted = registry.get_or_emplace<PlayerInpStndVlcyComponent>(figure);
        granted.vel_x = vel_x;
        granted.vel_z = vel_z;
    });

    /**
     * PASS 2: the mark, stated anew. Every tag comes off, then exactly the rows that carry the mark now lay it.
     */
    registry.clear<PlayerStndGrndTag>();
    hub::row_each(registry, "HUB_PHYS_STAND"_hs, "HUB_PHYS_STAND_GRND"_hs, [&](ecs::Entity figure) {
        registry.emplace_or_replace<PlayerStndGrndTag>(figure);
    });

    /**
     * PASS 3: a placed figure whose answer was withdrawn keeps none. The view walks the placements, so taking the
     * answer off does not change its set.
     */
    for (auto figure : hub::without_row(registry.view<PlayerStaPlcmComponent>(), registry, "HUB_PHYS_STAND"_hs)) {
        if (registry.remove<PlayerInpStndComponent, PlayerInpStndVlcyComponent>(figure) > 0) {
            log::debug("[PlayerHubStndSyncSystem] figure {} is no body of ase-physics any more",
                       static_cast<uint32_t>(figure));
        }
    }
}

void PlayerHubStndSyncSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubStndSyncSystem] Stopped");
}

}  // namespace ase::player
