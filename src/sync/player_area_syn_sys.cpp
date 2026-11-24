/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_area_syn_sys.cpp
 * @brief       PlayerAreaSynSystem - lays the plate anchor and the raster edges of every bound figure
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    input
 * @schedule    Ingestion
 * @design      DSGN_019
 * @usecase     usecase:101612 - keep figure in loaded area
 * @actor       Game Areal Plate (AREAL)
 * @created     2026-11-13
 * @modified    2026-11-24
 * @version     00.01.45.00095 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.7: figure and player camera stay in the area)
 *
 *   [ase-terrain levels the ground of the area plate and files its anchor on the plate itself: TRN_FLAT_HGT,
 *    TRN_FLAT_X and TRN_FLAT_Z (TerrainHubFlatPubSystem); the levelled cells reach the ase-gis elevation
 *    raster through HUB_TERN_MCRO]
 *   [ase-pl-avatar binds the figure to that plate: row HUB_PLR_AREA, field HUB_PLR_AREA_PLATE]
 *   [ase-physics asks ase-gis for the loaded raster around the walking body, which is the figure
 *    (HUB_GIS_GRND_REQ, PhysicsHubBndyOrdrSystem); ase-gis answers HUB_GIS_GRND on the same entity]
 *          │
 *          ▼
 *   ┌──────────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerAreaSynSystem                                │
 *   │  READS:  HUB_PLR_AREA_PLATE, PlayerStaWrldComponent,             │
 *   │          TRN_FLAT_* on the plate, HUB_GIS_GRND on the figure     │
 *   │  WRITES: PlayerInpAreaComponent, PlayerInpGrndComponent          │
 *   └──────────────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   [PlayerSimBndySystem in Dynamics keeps the settled place inside the raster edges; PlayerSimWalkSystem lays
 *    the walking frame on the plate anchor]
 *
 * HUB Pattern (ARCH_ASE_HUB)
 *
 * READS (from Hub):
 *   row HUB_PLR_AREA, field HUB_PLR_AREA_PLATE                     (per figure)
 *   TRN_FLAT_HGT, TRN_FLAT_X, TRN_FLAT_Z                           (per plate, only once measured)
 *   row HUB_GIS_GRND, fields HUB_GIS_GRND_WEST, _EAST, _SOUTH, _NORTH (adopted on the figure by ase-gis, the
 *     answer to the HUB_GIS_GRND_REQ ase-physics files on the walking body, which is the figure)
 *   row HUB_AVTR_RIG, field HUB_AVTR_RIG_RADIUS_M                  (per figure, laid by ase-pl-avatar)
 *
 * WRITES (to Hub):
 *   none
 *
 * WRITES (to Components):
 *   PlayerInpAreaComponent.level, x, z, plate - the anchor of the bound plate
 *   PlayerInpGrndComponent.west, east, south, north - the edges of the loaded raster around the figure
 *   PlayerInpGrndComponent.radius - the capsule radius of the walking body, the margin of the glass
 *   PlayerAreaBndyTag - taken off together with the area: a figure without an area stands on no rim
 *
 * THE BOUND IS RASTER, NEVER A CIRCLE. PlayerSimBndySystem keeps the figure inside the four edges of
 * PlayerInpGrndComponent, the chunk borders of the loaded elevation raster ase-gis walks from the chunk of the
 * figure; ase-physics holds the walking body at the very same edges (PhysicsWalkBndySystem), so figure and body
 * meet one border.
 *
 * THE ANCHOR IS READ ON THE PLATE. The plate comes from the binding and is never found by place, so several
 * plates in one 32 m chunk never mix (B0447). The anchor is the same set PhysicsHubGrndSyncSystem reads for the
 * frame of a body: the declared 0.0 of an unmeasured value is no height and no place, so every value is asked
 * with hub::is_measured first. How far the ground is levelled is no number of the plate: the levelled cells
 * stand as metres in the ase-gis layer GIS_CAT_LITHO_TRN, and every body asks the cell under it (HUB_GIS_ELEV).
 *
 * NO ANSWER IS NO AREA. Until ase-terrain has filed the anchor, and until ase-gis has answered the raster, the
 * figure walks unbounded. A lost area is reported once, with the plate, when the component goes; afterwards the
 * pass is quiet until the area returns.
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
#include <ase/player/systems/sync/player_area_syn_sys.hpp>
// Components from same module
#include <ase/player/components/tag/player_spnd_tag.hpp>
#include <ase/player/components/input/player_inp_area_comp.hpp>
#include <ase/player/components/input/player_inp_grnd_comp.hpp>
#include <ase/player/components/state/player_sta_wrld_comp.hpp>
#include <ase/player/components/tag/player_area_bndy_tag.hpp>
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

void PlayerAreaSynSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerAreaSynSystem] Started");
}

void PlayerAreaSynSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * PASS: every figure on the roster. Adding or removing the area and edge components does not touch the
     * storage the view walks.
     */
    for (auto figure : registry.view<PlayerSpndTag>()) {
        const auto figure_id = static_cast<uint32_t>(figure);

        /**
         * STEP 1: Bound? A figure the host never bound to a plate walks unbounded; the components it carried from
         * an earlier binding go with the binding.
         */
        if (!hub::row_has(registry, figure, "HUB_PLR_AREA"_hs)) {
            registry.remove<PlayerInpAreaComponent, PlayerInpGrndComponent, PlayerAreaBndyTag>(figure);
            continue;
        }
        uint32_t plate = types::InvalidEntityId;
        if (!hub::try_get_u32(registry, figure_id, "HUB_PLR_AREA_PLATE"_hs, plate) || !types::is_val_entity(plate)) {
            log::error(log::ERR::CAT::HUB_NOT_FOUND, "PlayerAreaSynSystem", figure_id, "HUB_PLR_AREA_PLATE");
            registry.remove<PlayerInpAreaComponent, PlayerInpGrndComponent, PlayerAreaBndyTag>(figure);
            continue;
        }

        /**
         * STEP 2: The world place of the figure is required: a bound figure without it is a broken spawn and walks
         * unbounded.
         */
        const auto* wrld = registry.try_get<PlayerStaWrldComponent>(figure);
        if (wrld == nullptr) {
            log::error(log::ERR::CAT::COMPONENT_MISSING, "PlayerAreaSynSystem", figure_id, "PlayerStaWrldComponent");
            registry.remove<PlayerInpAreaComponent, PlayerInpGrndComponent, PlayerAreaBndyTag>(figure);
            continue;
        }

        /**
         * STEP 3: The anchor of the bound plate, read on the plate itself where ase-terrain files it
         * (TerrainHubFlatPubSystem), the same read PhysicsHubGrndSyncSystem makes for the frame of a body. The
         * declared 0.0 of an unmeasured value is no height and no place: until all three are measured the figure
         * carries no anchor, and the loss is named once.
         */
        if (!hub::is_measured(registry, plate, "TRN_FLAT_HGT"_hs) || !hub::is_measured(registry, plate, "TRN_FLAT_X"_hs) ||
            !hub::is_measured(registry, plate, "TRN_FLAT_Z"_hs)) {
            if (registry.all_of<PlayerInpAreaComponent>(figure)) {
                log::warn(log::WRN::CAT::HUB_NOT_FOUND, "PlayerAreaSynSystem", plate, "TRN_FLAT_HGT");
                registry.remove<PlayerInpAreaComponent, PlayerInpGrndComponent, PlayerAreaBndyTag>(figure);
            }
            continue;
        }

        /**
         * STEP 4: Read and validate each value as it is read. A value that is no number is a defect of the writer;
         * the figure walks unbounded with it.
         */
        const float level = hub::get(registry, plate, "TRN_FLAT_HGT"_hs);
        if (!types::is_val_float(level)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerAreaSynSystem", plate, "TRN_FLAT_HGT", level);
            registry.remove<PlayerInpAreaComponent, PlayerInpGrndComponent, PlayerAreaBndyTag>(figure);
            continue;
        }
        const float x = hub::get(registry, plate, "TRN_FLAT_X"_hs);
        if (!types::is_val_float(x)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerAreaSynSystem", plate, "TRN_FLAT_X", x);
            registry.remove<PlayerInpAreaComponent, PlayerInpGrndComponent, PlayerAreaBndyTag>(figure);
            continue;
        }
        const float z = hub::get(registry, plate, "TRN_FLAT_Z"_hs);
        if (!types::is_val_float(z)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerAreaSynSystem", plate, "TRN_FLAT_Z", z);
            registry.remove<PlayerInpAreaComponent, PlayerInpGrndComponent, PlayerAreaBndyTag>(figure);
            continue;
        }

        /**
         * STEP 5: Lay the anchor on the figure; the first arrival is named with its plate.
         */
        if (!registry.all_of<PlayerInpAreaComponent>(figure)) {
            log::info("[PlayerAreaSynSystem] figure {} walks on area plate {} (anchor {} {}, level {}, figure at {} {})",
                      figure_id, plate, x, z, level, wrld->x, wrld->y);
        }
        auto& area = registry.get_or_emplace<PlayerInpAreaComponent>(figure);
        area.level = level;
        area.x = x;
        area.z = z;
        area.plate = plate;

        /**
         * STEP 6: The edges of the loaded raster. ase-physics asks ase-gis for them on the walking body, and the
         * walking body is the figure itself (HUB_GIS_GRND_REQ, PhysicsHubBndyOrdrSystem); ase-gis answers on the
         * same entity, so the figure reads them there without a question of its own. No answer yet is no bound:
         * the component goes and the figure walks unbounded until the raster around it is answered.
         */
        if (!hub::row_has(registry, figure, "HUB_GIS_GRND"_hs)) {
            registry.remove<PlayerInpGrndComponent>(figure);
            continue;
        }
        float west = 0.0f;
        if (!hub::try_get_f32(registry, figure_id, "HUB_GIS_GRND_WEST"_hs, west) || !types::is_val_float(west)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerAreaSynSystem", figure_id, "HUB_GIS_GRND_WEST", west);
            registry.remove<PlayerInpGrndComponent>(figure);
            continue;
        }
        float east = 0.0f;
        if (!hub::try_get_f32(registry, figure_id, "HUB_GIS_GRND_EAST"_hs, east) || !types::is_val_float(east)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerAreaSynSystem", figure_id, "HUB_GIS_GRND_EAST", east);
            registry.remove<PlayerInpGrndComponent>(figure);
            continue;
        }
        float south = 0.0f;
        if (!hub::try_get_f32(registry, figure_id, "HUB_GIS_GRND_SOUTH"_hs, south) || !types::is_val_float(south)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerAreaSynSystem", figure_id, "HUB_GIS_GRND_SOUTH", south);
            registry.remove<PlayerInpGrndComponent>(figure);
            continue;
        }
        float north = 0.0f;
        if (!hub::try_get_f32(registry, figure_id, "HUB_GIS_GRND_NORTH"_hs, north) || !types::is_val_float(north)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerAreaSynSystem", figure_id, "HUB_GIS_GRND_NORTH", north);
            registry.remove<PlayerInpGrndComponent>(figure);
            continue;
        }
        if (!registry.all_of<PlayerInpGrndComponent>(figure)) {
            log::info("[PlayerAreaSynSystem] figure {} bounded by the loaded raster (west {} east {} south {} north {})",
                      figure_id, west, east, south, north);
        }
        auto& grnd = registry.get_or_emplace<PlayerInpGrndComponent>(figure);
        grnd.west = west;
        grnd.east = east;
        grnd.south = south;
        grnd.north = north;

        /**
         * STEP 7: The capsule radius of the body, the measure ase-pl-avatar laid on the figure (HUB_AVTR_RIG) and
         * PhysicsHubWalkSyncSystem gives the capsule. Its glass holds the centre one radius inside the edges, so the
         * box of PlayerSimBndySystem keeps the place at least that far in. An unmeasured skeleton is a point body:
         * radius 0. A measure that is no length keeps the radius the figure had.
         */
        if (!hub::row_has(registry, figure, "HUB_AVTR_RIG"_hs)) {
            grnd.radius = 0.0f;
            continue;
        }
        float radius = 0.0f;
        if (!hub::try_get_f32(registry, figure_id, "HUB_AVTR_RIG_RADIUS_M"_hs, radius) || !types::is_val_float(radius) ||
            types::is_neg_float(radius)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerAreaSynSystem", figure_id, "HUB_AVTR_RIG_RADIUS_M", radius);
            continue;
        }
        grnd.radius = radius;
    }
}

void PlayerAreaSynSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerAreaSynSystem] Stopped");
}

}  // namespace ase::player
