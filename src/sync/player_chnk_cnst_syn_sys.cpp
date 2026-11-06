/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_chnk_cnst_syn_sys.cpp
 * @brief       PlayerChnkCnstSynSystem - lays the chunk edge of the terrain protocol on the manager row
 * @description Hub READ only. The player cuts the world into chunks of the edge the terrain protocol
 *              sets; this system fills one input component with it and removes it when the constant
 *              is missing or not positive.
 *
 * chnk = chunk, cnst = constant
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    input
 * @parity      server_only
 * @schedule    Ingestion
 * @usecase     usecase:100384 - convert place to world metres
 * @actor       Grid Body Keeper (GRIDBODKEE)
 * @created     2026-11-01
 * @modified    2026-11-01
 * @version     1.0.0
 *
 * CAUSAL CHAIN (the chunk edge of the player)
 *
 *   [ase-hub, HubIniCnstSystem]
 *          │
 *          │ lays the TER_CONST_CHUNK_SIZE row of hub_constants.json on GLOBAL
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerChnkCnstSynSystem                       │
 *   │                                                             │
 *   │  READS (from Hub):                                          │
 *   │    → "TER_CONST_CHUNK_SIZE"_hs                              │
 *   │                                                             │
 *   │  READS (Components):                                        │
 *   │    → PlayerMgrTag (the manager row)                         │
 *   │                                                             │
 *   │  WRITES (Components):                                       │
 *   │    → PlayerInpChnkCnstComponent                             │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ the chunk edge, without the hub
 *          ▼
 *   [PlayerLifeSpwnSystem, PlayerSimPhysSystem, PlayerSimWrldSystem, PlayerAccMovSystem,
 *    PlayerMigSerSystem, PlayerMigAplySystem, PlayerSyncInpSystem]
 *
 * HUB Pattern (MIG_ASE_HUB_API v2.0):
 *
 * READS (from Hub):
 *   "TER_CONST_CHUNK_SIZE"_hs → edge of one terrain chunk in metres, global
 *
 * WRITES (to Hub):
 *   None - the sync half fills an input component
 *
 * THE COMPONENT IS THE PRESENCE PROBE. It is laid only while the constant is found and positive,
 * and taken off as soon as it is not; a reader without it converts nothing. An edge of zero would
 * divide by zero, a negative one would turn every chunk address around.
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
#include <ase/player/systems/sync/player_chnk_cnst_syn_sys.hpp>
// Components from same module
#include <ase/player/components/tag/player_mgr_tag.hpp>
#include <ase/player/components/input/player_inp_chnk_cnst_comp.hpp>
// ase-types (Layer 0) for NOT_FOUND / positive-float SSOT predicates
#include <ase/types/types.hpp>
// Hub for O(1) API
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>

namespace ase::player {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

// No helper functions needed - one Hub read onto one bridge field of the manager row.

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerChnkCnstSynSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerChnkCnstSynSystem] Started");
}

void PlayerChnkCnstSynSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Read the edge. A missing row is reported by name, an edge that is not positive is
     * reported as invalid; both take the input row off the manager, so no reader cuts the world
     * on last tick's value or divides by zero.
     */
    float chunk_size = hub::get(registry, hub::GLOBAL, "TER_CONST_CHUNK_SIZE"_hs);
    if (ase::types::is_not_found(chunk_size)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "PlayerChnkCnstSynSystem", "TER_CONST_CHUNK_SIZE");
        for (auto entity : registry.view<PlayerMgrTag>()) {
            registry.remove<PlayerInpChnkCnstComponent>(entity);
        }
        return;
    }
    if (!ase::types::is_pos_float(chunk_size)) {
        log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerChnkCnstSynSystem", "TER_CONST_CHUNK_SIZE",
                  chunk_size);
        for (auto entity : registry.view<PlayerMgrTag>()) {
            registry.remove<PlayerInpChnkCnstComponent>(entity);
        }
        return;
    }

    /**
     * STEP 2: Lay the edge on every manager row.
     */
    for (auto entity : registry.view<PlayerMgrTag>()) {
        auto& cnst = registry.get_or_emplace<PlayerInpChnkCnstComponent>(entity);
        cnst.chunk_size = chunk_size;
    }
}

void PlayerChnkCnstSynSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerChnkCnstSynSystem] Stopped");
}

}  // namespace ase::player
