/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sim_walk_sys.cpp
 * @brief       PlayerSimWalkSystem - files the step the figure wishes to walk, in the frame ase-physics walks it in
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-11-14
 * @modified    2026-11-14
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.13, DoD 07-K: ase-player writes its intent, ase-physics
 * resolves gravity and ground contact)
 *
 *   [PlayerCtrlMovSystem set the horizontal velocity; teleport, knockback, migration and PlayerSimBndySystem
 *    marked PlayerWalkRplcTag where they put the figure somewhere; PlayerAreaSynSystem laid the plate]
 *          │
 *          ▼
 *   ┌──────────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSimWalkSystem (Dynamics)                     │
 *   │  READS (from Components):                                        │
 *   │    PlayerInpAreaComponent, PlayerStaPosComponent,                │
 *   │    PlayerStaVelComponent, PlayerInpChnkCnstComponent             │
 *   │  WRITES (to Components):                                         │
 *   │    PlayerStaFrmComponent, PlayerStaPlcmComponent,                │
 *   │    PlayerBufWalkComponent, PlayerBufWalkFrmComponent             │
 *   └──────────────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   [PlayerHubWalkSystem writes HUB_PHYS_WALK; ase-physics walks the body and answers with HUB_PHYS_STAND]
 *
 * THE FRAME. Under a plate the origin is the plate centre and the level the plate level; on no plate the origin is
 * the corner of the chunk the figure stands in, at level 0. Either way the origin is stored as chunk plus metres
 * within it, so no world coordinate is held as one large float. ase-physics lays no ground from it: it asks the
 * elevation cells of ase-gis at origin plus place (PhysicsGrndCalcSystem) and holds the capsule against the quads
 * they answer (PhysicsWalkCntcSystem).
 *
 * WHEN THE FRAME IS LAID AGAIN. A figure without a frame, a figure whose plate or plate frame changed, a figure
 * that left its plate, an unbound figure that walked more than PLR_WALK_FRAME_REACH_CHUNKS chunks from its frame
 * corner, and every figure another system marked PlayerWalkRplcTag. Laying raises the placement count.
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   (none - uses SYN pattern, reads PlayerInpAreaComponent laid by PlayerAreaSynSystem)
 *
 * WRITES (to Hub for other modules):
 *   (none - uses SYN pattern, writes PlayerBufWalkComponent and PlayerBufWalkFrmComponent; PlayerHubWalkSystem
 *   writes the row HUB_PHYS_WALK)
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
#include <ase/player/systems/simulation/player_sim_walk_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/buffer/player_buf_walk_comp.hpp>
#include <ase/player/components/buffer/player_buf_walk_frm_comp.hpp>
#include <ase/player/components/input/player_inp_area_comp.hpp>
#include <ase/player/components/input/player_inp_chnk_cnst_comp.hpp>
#include <ase/player/components/state/player_sta_frm_comp.hpp>
#include <ase/player/components/state/player_sta_phys_comp.hpp>
#include <ase/player/components/state/player_sta_plcm_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/tag/player_mgr_tag.hpp>
#include <ase/player/components/tag/player_walk_rplc_tag.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Entity sentinel
#include <ase/types/types.hpp>
// Logging
#include <ase/log/log.hpp>
// Math
#include <ase/math/math.hpp>

#include <cstdint>

namespace ase::player {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

// No helpers - each axis is carried inline, so the validator sees every formula

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerSimWalkSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimWalkSystem] Started");
}

void PlayerSimWalkSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: The chunk edge is the terrain protocol's TER_CONST_CHUNK_SIZE, laid on the manager row by
     * PlayerChnkCnstSynSystem. Without it no frame is laid: a frame on a guessed edge would file every step at a
     * wrong place.
     */
    float chunk_edge = 0.0f;
    bool edge_loaded = false;
    for (auto [mgr, edge] : registry.view<PlayerMgrTag, PlayerInpChnkCnstComponent>().each()) {
        (void)mgr;
        chunk_edge = edge.chunk_size;
        edge_loaded = true;
    }
    if (!edge_loaded) {
        return;
    }

    /**
     * PASS A: a walking figure without a frame has never been placed.
     */
    for (auto [figure, pos, vel, physics] :
         registry.view<PlayerStaPosComponent, PlayerStaVelComponent, PlayerStaPhysComponent>(
             entt::exclude<PlayerStaFrmComponent>).each()) {
        (void)pos;
        (void)vel;
        (void)physics;
        registry.emplace_or_replace<PlayerWalkRplcTag>(figure);
    }

    /**
     * PASS B: a figure on a plate keeps its frame while plate, plate centre and plate level stay. The expected
     * frame is computed with the same formula PASS D lays it with, so an unchanged plate compares equal.
     */
    for (auto [figure, area, frame, placement] :
         registry.view<PlayerInpAreaComponent, PlayerStaFrmComponent, PlayerStaPlcmComponent>().each()) {
        const float cells_x = math::floor(area.x / chunk_edge);
        const float cells_z = math::floor(area.z / chunk_edge);
        const int32_t expect_chunk_x = static_cast<int32_t>(cells_x);
        const int32_t expect_chunk_z = static_cast<int32_t>(cells_z);
        const float expect_off_x = area.x - cells_x * chunk_edge;
        const float expect_off_z = area.z - cells_z * chunk_edge;

        const bool same_plate = placement.owner == area.plate;
        const bool same_chunk = frame.chunk_x == expect_chunk_x && frame.chunk_z == expect_chunk_z;
        const bool same_off = frame.off_x == expect_off_x && frame.off_z == expect_off_z;
        const bool same_level = frame.level == area.level;
        if (same_plate && same_chunk && same_off && same_level) {
            continue;
        }
        registry.emplace_or_replace<PlayerWalkRplcTag>(figure);
    }

    /**
     * PASS C: a figure on no plate. A frame that still names a plate was laid while the figure stood on one; a
     * figure that walked more than PLR_WALK_FRAME_REACH_CHUNKS chunks from its frame corner gets a nearer corner.
     */
    for (auto [figure, frame, placement, pos] :
         registry.view<PlayerStaFrmComponent, PlayerStaPlcmComponent, PlayerStaPosComponent>(
             entt::exclude<PlayerInpAreaComponent>).each()) {
        const bool left_plate = types::is_val_entity(placement.owner);
        const int32_t reach_x = math::abs(pos.chunk_x - frame.chunk_x);
        const int32_t reach_z = math::abs(pos.chunk_z - frame.chunk_z);
        const bool out_of_reach = reach_x > PLR_WALK_FRAME_REACH_CHUNKS || reach_z > PLR_WALK_FRAME_REACH_CHUNKS;
        if (!left_plate && !out_of_reach) {
            continue;
        }
        registry.emplace_or_replace<PlayerWalkRplcTag>(figure);
    }

    /**
     * PASS D1: lay the plate frame. The view carries the tag and the area, never the frame or the placement it
     * lays; the tag is cleared once after both passes.
     */
    for (auto [figure, area] : registry.view<PlayerWalkRplcTag, PlayerInpAreaComponent>().each()) {
        const float cells_x = math::floor(area.x / chunk_edge);
        const float cells_z = math::floor(area.z / chunk_edge);
        auto& frame = registry.get_or_emplace<PlayerStaFrmComponent>(figure);
        frame.chunk_x = static_cast<int32_t>(cells_x);
        frame.chunk_z = static_cast<int32_t>(cells_z);
        frame.off_x = area.x - cells_x * chunk_edge;
        frame.off_z = area.z - cells_z * chunk_edge;
        frame.level = area.level;

        auto& placement = registry.get_or_emplace<PlayerStaPlcmComponent>(figure);
        placement.owner = area.plate;
        placement.seq += 1;
        log::debug("[PlayerSimWalkSystem] Figure {} placed on plate {} (placement {})",
                   static_cast<uint32_t>(figure), area.plate, placement.seq);
    }

    /**
     * PASS D2: lay the chunk-corner frame of a figure on no plate.
     */
    for (auto [figure, pos] :
         registry.view<PlayerWalkRplcTag, PlayerStaPosComponent>(entt::exclude<PlayerInpAreaComponent>).each()) {
        auto& frame = registry.get_or_emplace<PlayerStaFrmComponent>(figure);
        frame.chunk_x = pos.chunk_x;
        frame.chunk_z = pos.chunk_z;
        frame.off_x = 0.0f;
        frame.off_z = 0.0f;
        frame.level = 0.0f;

        auto& placement = registry.get_or_emplace<PlayerStaPlcmComponent>(figure);
        placement.owner = types::InvalidEntityId;
        placement.seq += 1;
        log::debug("[PlayerSimWalkSystem] Figure {} placed on chunk {},{} (placement {})",
                   static_cast<uint32_t>(figure), pos.chunk_x, pos.chunk_z, placement.seq);
    }
    registry.clear<PlayerWalkRplcTag>();

    /**
     * PASS E: file the wish relative to the frame. The place counts whole chunks from the frame chunk, the
     * height from the frame level; the frame origin goes along, so ase-physics asks the ase-gis cells under the
     * figure's world place.
     */
    for (auto [figure, frame, placement, pos, vel] :
         registry.view<PlayerStaFrmComponent, PlayerStaPlcmComponent, PlayerStaPosComponent,
                       PlayerStaVelComponent>().each()) {
        (void)placement;
        auto& walk = registry.get_or_emplace<PlayerBufWalkComponent>(figure);
        walk.x = static_cast<float>(pos.chunk_x - frame.chunk_x) * chunk_edge + pos.local_x - frame.off_x;
        walk.z = static_cast<float>(pos.chunk_z - frame.chunk_z) * chunk_edge + pos.local_z - frame.off_z;
        walk.y = pos.y - frame.level;
        walk.vel_x = vel.vx;
        walk.vel_z = vel.vz;

        auto& origin = registry.get_or_emplace<PlayerBufWalkFrmComponent>(figure);
        origin.origin_x = static_cast<float>(frame.chunk_x) * chunk_edge + frame.off_x;
        origin.origin_z = static_cast<float>(frame.chunk_z) * chunk_edge + frame.off_z;
    }
}

void PlayerSimWalkSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimWalkSystem] Stopped");
}

}  // namespace ase::player
