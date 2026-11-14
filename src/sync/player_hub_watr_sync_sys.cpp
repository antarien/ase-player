/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_watr_sync_sys.cpp
 * @brief       PlayerHubWatrSyncSystem - lays the water depth under every figure onto the figure
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
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_ASE_HYDRO, "Wasser je Ort", item 6; B0358)
 *
 *   [ase-hydro writes the depth of every chunk into the layer GIS_LYR_HYD_DEPTH; GisQryHydCalcSystem of
 *    ase-gis maps the place PLR_POS_X/PLR_POS_Y of every figure on HUB_PLR_SPND to its chunk cell, and
 *    GisQryHydHubWrtSystem writes GIS_HYD_CELL_DEPTH on the figure]
 *          │
 *          ▼
 *   ┌──────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubWatrSyncSystem (Reception)            │
 *   │  READS:  GIS_HYD_CELL_DEPTH on every figure                  │
 *   │  WRITES: PlayerInpWatrComponent.depth_m                      │
 *   └──────────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   [PlayerStaStsSystem in Dynamics lays PlayerStndWatrTag while the depth lies above PLAYER_WATR_DRY_M]
 *
 * HUB Pattern (ARCH_ASE_HUB_API)
 *
 * READS (from Hub, per figure):
 *   GIS_HYD_CELL_DEPTH (hub::is_measured, hub::get), owner = the figure entity (PlayerSpndTag)
 *
 * WRITES (to Components):
 *   PlayerInpWatrComponent.depth_m
 *
 * A DEPTH NOT MEASURED IS NO DEPTH. ase-gis writes nothing for a figure whose cell is not mapped yet, and the
 * figure keeps no water from an earlier pass: the component comes off with the measurement, and only the loss
 * of a measurement that stood is reported. The view walks the figures (PlayerSpndTag), not the component, so
 * laying or taking it never changes the set this pass walks.
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
#include <ase/player/systems/sync/player_hub_watr_sync_sys.hpp>
// Components from same module
#include <ase/player/components/input/player_inp_watr_comp.hpp>
#include <ase/player/components/tag/player_spnd_tag.hpp>
// types.hpp for the depth bounds
#include <ase/player/types.hpp>
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

void PlayerHubWatrSyncSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubWatrSyncSystem] Started");
}

void PlayerHubWatrSyncSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * Every figure on the star carries PlayerSpndTag (PlayerHubSpndSystem adopts it as HUB_PLR_SPND), and that
     * row is what ase-gis maps; the view over the tag IS the set of figures it answers for.
     */
    for (auto figure : registry.view<PlayerSpndTag>()) {
        auto owner = static_cast<uint32_t>(figure);
        const bool had_depth = registry.all_of<PlayerInpWatrComponent>(figure);

        /**
         * STEP 1: Is the depth under this figure MEASURED? Absence withdraws the component; only the loss of a
         * measurement that stood is reported.
         */
        if (!hub::is_measured(registry, owner, "GIS_HYD_CELL_DEPTH"_hs)) {
            if (had_depth) {
                log::warn(log::WRN::CAT::HUB_NOT_FOUND, "PlayerHubWatrSyncSystem", owner,
                          "GIS_HYD_CELL_DEPTH", "no longer measured, the figure stands in no known water");
                registry.remove<PlayerInpWatrComponent>(figure);
            }
            continue;
        }

        /**
         * STEP 2: Read and validate - a depth in m, never negative (hub_metrics.json range)
         */
        float depth_m = hub::get(registry, owner, "GIS_HYD_CELL_DEPTH"_hs);
        if (!types::is_val_float(depth_m) ||
            !types::is_in_rng_f(depth_m, PLAYER_WATR_DEPTH_MIN_M, PLAYER_WATR_DEPTH_MAX_M)) {
            log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "PlayerHubWatrSyncSystem", owner,
                      "GIS_HYD_CELL_DEPTH", depth_m, PLAYER_WATR_DEPTH_MIN_M, PLAYER_WATR_DEPTH_MAX_M);
            continue;
        }

        /**
         * STEP 3: Lay the depth on the figure; its first appearance is the other edge and is reported once.
         */
        if (!had_depth) {
            log::debug("[PlayerHubWatrSyncSystem] figure {}: water depth measured, {} m", owner, depth_m);
        }
        registry.get_or_emplace<PlayerInpWatrComponent>(figure).depth_m = depth_m;
    }
}

void PlayerHubWatrSyncSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubWatrSyncSystem] Stopped");
}

}  // namespace ase::player
