/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_walk_sys.cpp
 * @brief       PlayerHubWalkSystem - writes the step every walking figure wishes as its HUB_PHYS_WALK row
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @category    hub
 * @schedule    Dynamics
 * @created     2026-11-14
 * @modified    2026-11-14
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.13, DoD 07-K: ase-player writes its intent as
 * HUB_PHYS_WALK, ase-physics resolves gravity and ground contact and answers with HUB_PHYS_STAND)
 *
 *   [PlayerSimWalkSystem filed the step in its frame: PlayerBufWalkComponent, PlayerBufWalkFrmComponent,
 *    PlayerStaPlcmComponent; PlayerCtrlMovSystem marked PlayerJumpReqTag on a jump]
 *          │
 *          ▼
 *   ┌──────────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubWalkSystem (Dynamics)                     │
 *   │  READS:  PlayerBufWalkComponent, PlayerBufWalkFrmComponent,      │
 *   │          PlayerStaPlcmComponent, PlayerJumpReqTag                │
 *   │  WRITES: row HUB_PHYS_WALK (adopt) and its ten fields            │
 *   └──────────────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   [PhysicsHubWalkSyncSystem of ase-physics lays the body; the answer comes back as HUB_PHYS_STAND]
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   (none - this system only writes; the answer is read by PlayerHubStndSyncSystem)
 *
 * WRITES (to Hub for other modules):
 *   row kind HUB_PHYS_WALK → hub::row_adopt onto the figure entity, idempotent
 *   HUB_PHYS_WALK_OWNER, HUB_PHYS_WALK_SEQ → hub::set_u32
 *   HUB_PHYS_WALK_ORIGIN_X, _ORIGIN_Z, _X, _Y, _Z, _VEL_X, _VEL_Z, _JUMP → hub::set_f32
 *
 * EVERY VALUE IS FILLED IN EVERY PASS from its one home, so ase-physics never reads a step older than the frame.
 * ase-physics reads the place only when the placement count moved or the body is new; between placements it
 * walks the body on from its own state and takes the velocity and the jump.
 *
 * THE AXIS ORDER. This module carries the height in y and spans the ground with x and z; HUB_PHYS_WALK names the
 * same axes, so no swap happens here.
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
#include <ase/player/systems/hub/player_hub_walk_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/buffer/player_buf_walk_comp.hpp>
#include <ase/player/components/buffer/player_buf_walk_frm_comp.hpp>
#include <ase/player/components/state/player_sta_plcm_comp.hpp>
#include <ase/player/components/tag/player_jump_req_tag.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Hub
#include <ase/hub/api.hpp>
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

// No helpers - every field is written inline from its one home

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerHubWalkSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubWalkSystem] Started");
}

void PlayerHubWalkSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * PASS 1: adopt the row and fill owner, frame origin, place, velocity and placement count of every figure
     * with a filed step.
     */
    for (auto [figure, walk, origin, placement] :
         registry.view<PlayerBufWalkComponent, PlayerBufWalkFrmComponent, PlayerStaPlcmComponent>().each()) {
        hub::row_adopt(registry, figure, "HUB_PHYS_WALK"_hs);
        const auto owner = static_cast<uint32_t>(figure);
        hub::set_u32(registry, owner, "HUB_PHYS_WALK_OWNER"_hs, placement.owner);
        hub::set_f32(registry, owner, "HUB_PHYS_WALK_ORIGIN_X"_hs, origin.origin_x);
        hub::set_f32(registry, owner, "HUB_PHYS_WALK_ORIGIN_Z"_hs, origin.origin_z);
        hub::set_f32(registry, owner, "HUB_PHYS_WALK_X"_hs, walk.x);
        hub::set_f32(registry, owner, "HUB_PHYS_WALK_Y"_hs, walk.y);
        hub::set_f32(registry, owner, "HUB_PHYS_WALK_Z"_hs, walk.z);
        hub::set_f32(registry, owner, "HUB_PHYS_WALK_VEL_X"_hs, walk.vel_x);
        hub::set_f32(registry, owner, "HUB_PHYS_WALK_VEL_Z"_hs, walk.vel_z);
        hub::set_u32(registry, owner, "HUB_PHYS_WALK_SEQ"_hs, placement.seq);
    }

    /**
     * PASS 2: the jump impulse, one view per state. A figure that asked to jump gets the impulse for this pass.
     */
    for (auto figure : registry.view<PlayerBufWalkComponent, PlayerStaPlcmComponent, PlayerJumpReqTag>()) {
        hub::set_f32(registry, static_cast<uint32_t>(figure), "HUB_PHYS_WALK_JUMP"_hs, MOVEMENT_DEFAULT_JUMP_IMPULSE);
    }

    /**
     * PASS 3: every other figure asks for no jump.
     */
    for (auto figure :
         registry.view<PlayerBufWalkComponent, PlayerStaPlcmComponent>(entt::exclude<PlayerJumpReqTag>)) {
        hub::set_f32(registry, static_cast<uint32_t>(figure), "HUB_PHYS_WALK_JUMP"_hs, 0.0f);
    }

    /**
     * STEP 4: the request is written; it is answered once.
     */
    registry.clear<PlayerJumpReqTag>();
}

void PlayerHubWalkSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubWalkSystem] Stopped");
}

}  // namespace ase::player
