/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_area_syn_sys.cpp
 * @brief       PlayerAreaSynSystem - lays the loaded area of its plate on every bound figure
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    input
 * @schedule    Ingestion
 * @design      DSGN_019
 * @usecase     usecase:101612 - keep figure in loaded area
 * @actor       Game Areal Plate (AREAL)
 * @created     2026-11-13
 * @modified    2026-11-14
 * @version     1.0.0
 *
 * CAUSAL CHAIN (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.7: figure and player camera stay in the area)
 *
 *   [ase-terrain levels the disc of the area plate and writes TRN_FLAT_* under it (TerrainHubFlatPubSystem)]
 *   [ase-pl-avatar binds the figure to that plate: row HUB_PLR_AREA, field HUB_PLR_AREA_PLATE]
 *          │
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerAreaSynSystem                           │
 *   │  READS:  HUB_PLR_AREA_PLATE, TRN_FLAT_* (owner = the plate) │
 *   │  WRITES: PlayerInpAreaComponent (on the figure)             │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   [PlayerSimBndySystem in Dynamics keeps the settled place inside the area; PlayerSimWalkSystem lays the
 *    walking frame on the plate]
 *
 * HUB Pattern (ARCH_ASE_HUB)
 *
 * READS (from Hub):
 *   row HUB_PLR_AREA, field HUB_PLR_AREA_PLATE        (per figure)
 *   TRN_FLAT_HGT, TRN_FLAT_RAD, TRN_FLAT_X, TRN_FLAT_Z (owner = the plate) - only once measured
 *
 * WRITES (to Components):
 *   PlayerInpAreaComponent.level, radius_m, x, z, plate
 *
 * MEASURED, NOT DECLARED. The four disc values stand in hub_metrics.json with 0.0, and 0.0 is a height, a
 * radius and a place, so the default of get() cannot say whether ase-terrain has levelled the plate. The row
 * is filled only when all four are measured; ase-terrain writes them together.
 *
 * A LOST AREA IS REPORTED ONCE. When the plate of a bound figure stops being measured, the row is removed and
 * the removal named with the plate; afterwards the figure has no row and the pass is quiet until it returns.
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
     * PASS: every figure on the roster. Adding or removing the area row does not touch the storage the view
     * walks.
     */
    for (auto figure : registry.view<PlayerSpndTag>()) {
        const auto figure_id = static_cast<uint32_t>(figure);

        /**
         * STEP 1: Bound? A figure the host never bound to a plate walks unbounded; a row it carried from an
         * earlier binding goes with the binding.
         */
        if (!hub::row_has(registry, figure, "HUB_PLR_AREA"_hs)) {
            registry.remove<PlayerInpAreaComponent>(figure);
            continue;
        }
        uint32_t plate = types::InvalidEntityId;
        if (!hub::try_get_u32(registry, figure_id, "HUB_PLR_AREA_PLATE"_hs, plate) || !types::is_val_entity(plate)) {
            log::error(log::ERR::CAT::HUB_NOT_FOUND, "PlayerAreaSynSystem", figure_id, "HUB_PLR_AREA_PLATE");
            registry.remove<PlayerInpAreaComponent>(figure);
            continue;
        }

        /**
         * STEP 2: Measured? The declared 0.0 of hub_metrics.json is a height, a radius and a place and is never
         * taken for an area. A row the figure carried is withdrawn, and the withdrawal named once.
         */
        if (!hub::is_measured(registry, plate, "TRN_FLAT_HGT"_hs) ||
            !hub::is_measured(registry, plate, "TRN_FLAT_RAD"_hs) ||
            !hub::is_measured(registry, plate, "TRN_FLAT_X"_hs) || !hub::is_measured(registry, plate, "TRN_FLAT_Z"_hs)) {
            if (registry.all_of<PlayerInpAreaComponent>(figure)) {
                log::warn(log::WRN::CAT::HUB_NOT_FOUND, "PlayerAreaSynSystem", plate, "TRN_FLAT_RAD");
                registry.remove<PlayerInpAreaComponent>(figure);
            }
            continue;
        }

        /**
         * STEP 3: Read and validate each value as it is read - ase-terrain wrote numbers, and a radius above
         * zero. An area that is no area is a defect of the writer; the figure walks unbounded with it.
         */
        const float level = hub::get(registry, plate, "TRN_FLAT_HGT"_hs);
        if (!types::is_val_float(level)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerAreaSynSystem", plate, "TRN_FLAT_HGT", level);
            registry.remove<PlayerInpAreaComponent>(figure);
            continue;
        }
        const float radius = hub::get(registry, plate, "TRN_FLAT_RAD"_hs);
        if (!types::is_val_float(radius) || !types::is_pos_float(radius)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerAreaSynSystem", plate, "TRN_FLAT_RAD", radius);
            registry.remove<PlayerInpAreaComponent>(figure);
            continue;
        }
        const float x = hub::get(registry, plate, "TRN_FLAT_X"_hs);
        if (!types::is_val_float(x)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerAreaSynSystem", plate, "TRN_FLAT_X", x);
            registry.remove<PlayerInpAreaComponent>(figure);
            continue;
        }
        const float z = hub::get(registry, plate, "TRN_FLAT_Z"_hs);
        if (!types::is_val_float(z)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerAreaSynSystem", plate, "TRN_FLAT_Z", z);
            registry.remove<PlayerInpAreaComponent>(figure);
            continue;
        }

        /**
         * STEP 4: Lay the area on the figure; the first arrival is named with its plate.
         */
        if (!registry.all_of<PlayerInpAreaComponent>(figure)) {
            log::info("[PlayerAreaSynSystem] figure {} walks on area plate {} (radius {} m)", figure_id, plate, radius);
        }
        auto& area = registry.get_or_emplace<PlayerInpAreaComponent>(figure);
        area.level = level;
        area.radius_m = radius;
        area.x = x;
        area.z = z;
        area.plate = plate;
    }
}

void PlayerAreaSynSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerAreaSynSystem] Stopped");
}

}  // namespace ase::player
