/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sim_bndy_sys.cpp
 * @brief       PlayerSimBndySystem - keeps every bound figure inside its loaded area
 * @description Pure calculation on the settled place: a figure beyond the chunk borders of the loaded ase-gis
 *              raster around it is set back onto them and loses the outward part of its walk wish and of its
 *              horizontal velocity; the walking body stays where the glass of ase-physics holds it.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101612 - keep figure in loaded area
 * @actor       Game Areal Plate (AREAL)
 * @design      DSGN_019
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-11-13
 * @modified    2026-11-23
 * @version     00.01.45.00095 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.7, DoD 07-C: the figure stays in the area)
 *
 *   [PlayerAreaSynSystem laid the edges of the loaded raster on the figure: PlayerInpGrndComponent]
 *   [PlayerSimStndSystem took the place ase-physics stood the body on: PlayerStaPosComponent,
 *    PlayerStaVelComponent; PlayerCtrlMovSystem, Cht, Tlpt and Knbk moved it for this frame]
 *          │
 *          ▼
 *   ┌────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSimBndySystem                          │
 *   │                                                            │
 *   │  READS:  PlayerInpGrndComponent, PlayerInpChnkCnstComponent │
 *   │  WRITES: PlayerStaPosComponent, PlayerStaVelComponent,     │
 *   │          PlayerStaVelWishComponent, PlayerAreaBndyTag      │
 *   └────────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   PlayerSimWalkSystem files the bounded wish to ase-physics, PlayerStaChnkSystem stamps the chunk address,
 *   PlayerSimWrldSystem converts the place into world metres.
 *
 * THE AXIS ORDER: the area is laid in the axis order of the star (x = PLR_POS_X, z = PLR_POS_Y); this module
 * spans the chunk grid with x and z and carries the height in y, so the second horizontal axis of the area is
 * the z axis of the place, the same swap PlayerSimWrldSystem makes.
 *
 * THE FLOOR IS NOT HERE. ase-physics stands the walking body on the elevation cells ase-gis measured under it
 * (PhysicsGrndCalcSystem asks them, PhysicsWalkCntcSystem holds the capsule against their quads) and answers with
 * HUB_PHYS_STAND; this system bounds the horizontal place only.
 *
 * THE RIM IS THE RASTER, NEVER A CIRCLE (usecase:101612, HUB_GIS_GRND). The figure is held inside the four chunk
 * borders of the loaded elevation raster, each moved in by PLR_AREA_EDGE_M or by the capsule radius of the body
 * (PlayerInpGrndComponent.radius), whichever is larger; every axis is set back on its own, and
 * only the outward part of the velocity goes, so a figure that runs against a border slides along it. The edges
 * move out when ase-gis loads a neighbouring chunk; ase-physics holds the walking body at the same borders
 * (PhysicsWalkBndySystem).
 *
 * NO LINE PER FRAME. A figure pushing against the rim is set back in every frame; that is the normal state of the
 * area, so the step writes no log line. The arrival and the loss of the area are named by PlayerAreaSynSystem.
 *
 * THE RIM STATE IS A TAG. PlayerAreaBndyTag stands on the figure in every frame it is set back and goes in the
 * frame it stands inside again; PlayerHubMotnSystem publishes it as HUB_PLR_MOTN_BNDY, and the drawing host
 * shows the line RAND DES AREALS of the mockup (07-G, cam.bound) and logs its edge.
 *
 * HUB Pattern (ARCH_ASE_HUB_API v2.0)
 *
 * READS (from Hub):   none - PlayerAreaSynSystem lays the area of the plate on the figure
 * WRITES (to Hub):    none - PlayerHubPosSystem publishes the bounded place
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
#include <ase/player/systems/simulation/player_sim_bndy_sys.hpp>
// Components from same module
#include <ase/player/components/input/player_inp_chnk_cnst_comp.hpp>
#include <ase/player/components/input/player_inp_grnd_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_vel_wish_comp.hpp>
#include <ase/player/components/tag/player_area_bndy_tag.hpp>
#include <ase/player/components/tag/player_mgr_tag.hpp>
// types.hpp for the margin the figure keeps from the rim
#include <ase/player/types.hpp>
// ase-types (Layer 0) for the positive-float predicate
#include <ase/types/types.hpp>
// Math from Layer 0
#include <ase/math/math.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

using namespace entt::literals;  // For "_hs hashed strings (Hub)

namespace ase::player {

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

// No helpers - the setback is one projection per figure in the loop that writes it

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerSimBndySystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimBndySystem] Started");
}

void PlayerSimBndySystem::tick(ecs::Registry& registry, float /*dt*/) {
    // The chunk edge is the terrain protocol's TER_CONST_CHUNK_SIZE, laid on the manager row by
    // PlayerChnkCnstSynSystem; without it no place is turned into world metres and none is bounded.
    float chunk_size = 0.0f;
    bool edge_loaded = false;
    for (auto [mgr, edge] : registry.view<PlayerMgrTag, PlayerInpChnkCnstComponent>().each()) {
        (void)mgr;
        chunk_size = edge.chunk_size;
        edge_loaded = true;
    }
    if (!edge_loaded) {
        return;
    }

    /**
     * PASS: every figure with the edges of its loaded raster. The view carries no type the loop adds or removes.
     */
    for (auto [figure, grnd, pos, vel] :
         registry.view<PlayerInpGrndComponent, PlayerStaPosComponent, PlayerStaVelComponent>().each()) {
        /**
         * STEP 1: The world place on the two horizontal axes, and the box the figure may stand in: the four chunk
         * borders of the loaded raster, each moved in by the margin. The margin is PLR_AREA_EDGE_M or the capsule
         * radius of the body, whichever is larger: the glass of PhysicsWalkBndySystem holds the centre one radius
         * inside the edge, and a box wider than the glass never trips, so the outward speed was never taken and the
         * figure reported a run the held body did not make (register B0915). A raster narrower than twice the
         * margin holds the figure on the middle line of that axis.
         */
        const float margin = math::max(PLR_AREA_EDGE_M, grnd.radius);
        const float world_x = static_cast<float>(pos.chunk_x) * chunk_size + pos.local_x;
        const float world_z = static_cast<float>(pos.chunk_z) * chunk_size + pos.local_z;
        const float mid_x = (grnd.west + grnd.east) * 0.5f;
        const float mid_z = (grnd.south + grnd.north) * 0.5f;
        const float min_x = math::min(grnd.west + margin, mid_x);
        const float max_x = math::max(grnd.east - margin, mid_x);
        const float min_z = math::min(grnd.south + margin, mid_z);
        const float max_z = math::max(grnd.north - margin, mid_z);

        /**
         * STEP 2: Inside the box the figure walks free. A figure this system set onto an edge stands ON it in the
         * next frame, a rounding step to either side: within PLR_AREA_RIM_M of an edge it is on the rim, the rim
         * state holds and nothing is set back. Without that band the state stood for the one frame of the set-back
         * and fell before the drawing host read it (measured 2026-11-17, case 07-G of the adapter test).
         */
        const bool inside_x = world_x >= min_x && world_x <= max_x;
        const bool inside_z = world_z >= min_z && world_z <= max_z;
        const bool free_x = world_x > min_x + PLR_AREA_RIM_M && world_x < max_x - PLR_AREA_RIM_M;
        const bool free_z = world_z > min_z + PLR_AREA_RIM_M && world_z < max_z - PLR_AREA_RIM_M;
        if (free_x && free_z) {
            registry.remove<PlayerAreaBndyTag>(figure);  // inside again: the rim state of this frame is off
            continue;
        }
        if (inside_x && inside_z) {
            registry.emplace_or_replace<PlayerAreaBndyTag>(figure);  // standing on an edge: the rim state holds
            continue;
        }

        /**
         * STEP 3: Set the place back onto the box, each axis on its own, then write chunk address and local metres
         * back from the world place, so the local metres stay inside [0, edge). The body ase-physics walks is NOT
         * laid again here: a new placement makes PhysicsHubWalkSyncSystem take the floor and the ground probe off
         * the body, and PhysicsWalkBndySystem holds the capsule a capsule radius inside the same raster edge, which
         * is the edge of this box or outside it (STEP 1). A body laid here was laid afresh in every frame, never had
         * a floor, never took a wish and stood fast on the glass (register B0821). The glass holds the body; this box holds the place the
         * figure reports, and a figure without a frame is laid by PASS A of PlayerSimWalkSystem in every tick.
         */
        const float bound_x = math::clamp(world_x, min_x, max_x);
        const float bound_z = math::clamp(world_z, min_z, max_z);
        const float cell_x = math::floor(bound_x / chunk_size);
        const float cell_z = math::floor(bound_z / chunk_size);
        pos.chunk_x = static_cast<int32_t>(cell_x);
        pos.chunk_z = static_cast<int32_t>(cell_z);
        pos.local_x = bound_x - cell_x * chunk_size;
        pos.local_z = bound_z - cell_z * chunk_size;
        registry.emplace_or_replace<PlayerAreaBndyTag>(figure);  // the rim state, published as HUB_PLR_MOTN_BNDY

        /**
         * STEP 4: Take away the outward part of the walk wish and of the granted speed on each axis the figure
         * crossed; the part along the edge stays, so a figure that runs against a chunk border slides along it. The
         * wish is what PlayerSimWalkSystem files to ase-physics: with its outward part gone the body stops pressing
         * against the glass of PhysicsWalkBndySystem and walks back in as soon as the stick points inward.
         */
        auto* wish = registry.try_get<PlayerStaVelWishComponent>(figure);
        const float out_x = (world_x > max_x) ? vel.vx : -vel.vx;
        if (!inside_x && types::is_pos_float(out_x)) {
            vel.vx = 0.0f;
        }
        const float out_z = (world_z > max_z) ? vel.vz : -vel.vz;
        if (!inside_z && types::is_pos_float(out_z)) {
            vel.vz = 0.0f;
        }
        if (wish == nullptr) {
            continue;  // a figure that carries no walk wish has none to bound
        }
        const float wish_out_x = (world_x > max_x) ? wish->vx : -wish->vx;
        if (!inside_x && types::is_pos_float(wish_out_x)) {
            wish->vx = 0.0f;
        }
        const float wish_out_z = (world_z > max_z) ? wish->vz : -wish->vz;
        if (!inside_z && types::is_pos_float(wish_out_z)) {
            wish->vz = 0.0f;
        }
    }
}

void PlayerSimBndySystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimBndySystem] Stopped");
}

}  // namespace ase::player
