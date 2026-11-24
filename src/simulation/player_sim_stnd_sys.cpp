/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sim_stnd_sys.cpp
 * @brief       PlayerSimStndSystem - takes the place ase-physics walked a figure to back into its chunk address
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-11-14
 * @modified    2026-11-23
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.13, DoD 07-K: the figure walks over slope and hollow and
 * falls into a dug hole, because ase-physics walks it)
 *
 *   [PlayerHubStndSyncSystem laid the answer of ase-physics as PlayerInpStndComponent,
 *    PlayerInpStndVlcyComponent and PlayerStndGrndTag; PlayerSimWalkSystem laid frame and placement count
 *    (PlayerStaFrmComponent, PlayerStaPlcmComponent)]
 *          │
 *          ▼
 *   ┌──────────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSimStndSystem (Dynamics)                     │
 *   │  READS (from Components):                                        │
 *   │    PlayerInpStndComponent, PlayerInpStndVlcyComponent,           │
 *   │    PlayerStaFrmComponent, PlayerStaPlcmComponent,                │
 *   │    PlayerStndGrndTag, PlayerInpChnkCnstComponent (manager row)   │
 *   │  WRITES (to Components):                                         │
 *   │    PlayerStaPosComponent, PlayerStaVelComponent.vx, vy, vz,      │
 *   │    PlayerStaPhysComponent.on_ground, PlayerDrtyTag               │
 *   └──────────────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   [PlayerCtrlMovSystem accelerates from the taken speed towards the stick; PlayerStaGaitSystem picks the gait
 *    from it; PlayerSimWalkSystem files the next step]
 *
 * THE SPEED IS THE ONE THE BODY HAS. The level speed of the answer is what ase-physics granted the wish: a wall
 * took the part that ran into it, the water of the cell resisted it by the share of its capsule under the water line
 * (PhysicsCurrSystem STEP 4, the law of every body in the brook). Taking it over makes the figure in the brook wade:
 * PlayerCtrlMovSystem accelerates from it, not from the
 * wish of the last tick, and the gait band reads the slower speed.
 *
 * THE FRAME. The answer stands in metres from the frame origin the figure filed: the origin is the chunk
 * PlayerStaFrmComponent names plus the metres within it, the height is measured from its level. Adding the
 * answer to the metres within the origin chunk and carrying whole chunks into the address keeps the local
 * metres inside [0, edge) and the sum small, so no step is swallowed by the float precision of a far place
 * (the cut of S2b 2026-08-11).
 *
 * THE GROUND IS STATED, NOT COMPUTED. Every taken answer first clears the ground contact; the second pass sets
 * it again on exactly the figures whose answer carries the mark HUB_PHYS_STAND_GRND, one view per state.
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   (none - uses SYN pattern, reads from PlayerInpStndComponent laid by PlayerHubStndSyncSystem)
 *
 * WRITES (to Hub for other modules):
 *   (none - uses SYN pattern, writes to Components only; PlayerSimWalkSystem and PlayerHubWalkSystem file the
 *   next step)
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
#include <ase/player/systems/simulation/player_sim_stnd_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/input/player_inp_chnk_cnst_comp.hpp>
#include <ase/player/components/input/player_inp_stnd_comp.hpp>
#include <ase/player/components/input/player_inp_stnd_vlcy_comp.hpp>
#include <ase/player/components/state/player_sta_frm_comp.hpp>
#include <ase/player/components/state/player_sta_plcm_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_phys_comp.hpp>
#include <ase/player/components/tag/player_drty_tag.hpp>
#include <ase/player/components/tag/player_mgr_tag.hpp>
#include <ase/player/components/tag/player_stnd_grnd_tag.hpp>
#include <ase/player/components/tag/player_walk_rplc_tag.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
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

void PlayerSimStndSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimStndSystem] Started");
}

void PlayerSimStndSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: The chunk edge is the terrain protocol's TER_CONST_CHUNK_SIZE, laid on the manager row by
     * PlayerChnkCnstSynSystem. Without it no answer is turned into a chunk address: a carry on a guessed edge
     * would put the figure into the wrong chunk.
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
     * PASS 1: every answer that follows the current placement. The view carries no type the loop adds or
     * removes; the dirty tag is not one of its types.
     */
    for (auto [figure, stand, granted, frame, placement, pos, vel, physics] :
         registry.view<PlayerInpStndComponent, PlayerInpStndVlcyComponent, PlayerStaFrmComponent,
                       PlayerStaPlcmComponent, PlayerStaPosComponent, PlayerStaVelComponent, PlayerStaPhysComponent>(
             entt::exclude<PlayerWalkRplcTag>).each()) {
        /**
         * STEP 2: An answer to an older placement follows a place the figure no longer stands at.
         */
        if (stand.seq != placement.seq) {
            continue;
        }

        /**
         * STEP 3: The place along each horizontal axis: the metres within the origin chunk plus the answer, the
         * whole chunks carried into the address, the rest the local metres in [0, edge).
         */
        const float along_x = frame.off_x + stand.x;
        const float cells_x = math::floor(along_x / chunk_edge);
        pos.chunk_x = frame.chunk_x + static_cast<int32_t>(cells_x);
        pos.local_x = along_x - cells_x * chunk_edge;

        const float along_z = frame.off_z + stand.z;
        const float cells_z = math::floor(along_z / chunk_edge);
        pos.chunk_z = frame.chunk_z + static_cast<int32_t>(cells_z);
        pos.local_z = along_z - cells_z * chunk_edge;

        /**
         * STEP 4: The height above the frame level and the speed of the body as ase-physics granted it, after wall
         * and water; the ground contact is cleared here and stated again in PASS 2.
         */
        pos.y = frame.level + stand.y;
        vel.vx = granted.vel_x;
        vel.vy = stand.vel_y;
        vel.vz = granted.vel_z;
        physics.on_ground = false;

        /**
         * STEP 5: A figure that moves changed its place for the star.
         */
        const float speed = math::sqrt(vel.vx * vel.vx + vel.vz * vel.vz + vel.vy * vel.vy);
        if (speed > MOVEMENT_DEFAULT_VELOCITY_EPSILON) {
            registry.emplace_or_replace<PlayerDrtyTag>(figure);
        }
    }

    /**
     * PASS 2: the figures whose taken answer says they stand on their ground.
     */
    for (auto [figure, stand, placement, physics] :
         registry.view<PlayerInpStndComponent, PlayerStaPlcmComponent, PlayerStaPhysComponent, PlayerStndGrndTag>(
             entt::exclude<PlayerWalkRplcTag>).each()) {
        (void)figure;
        if (stand.seq != placement.seq) {
            continue;
        }
        physics.on_ground = true;
    }
}

void PlayerSimStndSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimStndSystem] Stopped");
}

}  // namespace ase::player
