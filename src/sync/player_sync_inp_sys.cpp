/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sync_inp_sys.cpp
 * @brief       PlayerSyncInpSystem - Sync Hub input values to Input Component
 * @description SYN PATTERN: Reads Hub values and writes to PlayerInpExtComponent.
 *              Calculation systems read from PlayerInpExtComponent (no Hub access).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    input
 * @schedule    Synchronization
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_SYNC_INP: Player Input Synchronization)
 *
 *   [Hub Values from ase-input]
 *          │
 *          │ external input data
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSyncInpSystem           │
 *   │  (SERVER-ONLY - Hub access)                 │
 *   │                                             │
 *   │  READS (from Hub):                          │
 *   │    → "PLR_INP_FWD"_hs (forward input)       │
 *   │    → "PLR_INP_STR"_hs (strafe input)        │
 *   │    → "PLR_INP_SPRINT"_hs (sprint flag)      │
 *   │    → "PLR_INP_JUMP"_hs (jump flag)          │
 *   │    → "PLR_CAM_YAW"_hs (camera yaw)          │
 *   │    → "PLR_CAM_ORB"_hs (orbit mode)          │
 *   │    → "TRN_HGT_AT_POS"_hs (terrain height)   │
 *   │                                             │
 *   │  WRITES (to Components):                    │
 *   │    → PlayerInpExtComponent (all fields)     │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ input data in Component
 *          ▼
 *   PlayerCtrlInputCalcSystem (SHARED - no Hub!)
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub - ALL input values):
 *   "PLR_INP_FWD"_hs    → PlayerInpExtComponent.inp_fwd
 *   "PLR_INP_STR"_hs    → PlayerInpExtComponent.inp_str
 *   "PLR_INP_SPRINT"_hs → PlayerInpExtComponent.inp_sprint
 *   "PLR_INP_JUMP"_hs   → PlayerInpExtComponent.inp_jump
 *   "PLR_CAM_YAW"_hs    → PlayerInpExtComponent.cam_yaw
 *   "PLR_CAM_ORB"_hs    → PlayerInpExtComponent.cam_orb
 *   "TRN_HGT_AT_POS"_hs → PlayerInpExtComponent.trn_hgt
 *
 * WRITES (to Components only - no Hub writes):
 *   (none)
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
 * [ ] Cleanup System in Schedule::Last?
 * [ ] NO local arrays/vectors for collection?
 * [ ] 1 File = 1 System?
 * [ ] Folder structure matches convention?
 * [ ] components/, systems/, src/ have IDENTICAL subfolder structure?
 * [ ] Layer dependencies respected (no upward dependencies)?
 * [ ] NO inline nlohmann::json + .dump() in broadcast systems?
 * [ ] Serializer functions in anonymous namespace?
 * [ ] *NetBctReqSystem (Update) + *NetBctSndSystem (Replication) pattern?
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
 * [ ] on_start/on_stop: log::info with system name
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
#include <ase/player/systems/sync/player_sync_inp_sys.hpp>
// Components from same module
#include <ase/player/components/input/player_inp_ext_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Hub for HUB Pattern (SERVER-ONLY sync system)
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>

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

// No helper functions needed - all logic inlined in tick()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerSyncInpSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerSyncInpSystem] Started");
}

void PlayerSyncInpSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Process each player entity with position (all players)
     * Sync Hub input values to PlayerInpExtComponent
     */
    auto view = registry.view<PlayerStPosComponent>();

    for (auto [entity, pos] : view.each()) {
        uint32_t owner = static_cast<uint32_t>(entity);

        /**
         * STEP 2: Read all input values from Hub (HUB Pattern - READS)
         */
        float inp_fwd = hub::get(registry, owner, "PLR_INP_FWD"_hs);
        if (inp_fwd == hub::NOT_FOUND) {
            inp_fwd = 0.0f;
        }

        float inp_str = hub::get(registry, owner, "PLR_INP_STR"_hs);
        if (inp_str == hub::NOT_FOUND) {
            inp_str = 0.0f;
        }

        float inp_sprint = hub::get(registry, owner, "PLR_INP_SPRINT"_hs);
        if (inp_sprint == hub::NOT_FOUND) {
            inp_sprint = 0.0f;
        }

        float inp_jump = hub::get(registry, owner, "PLR_INP_JUMP"_hs);
        if (inp_jump == hub::NOT_FOUND) {
            inp_jump = 0.0f;
        }

        float cam_yaw = hub::get(registry, owner, "PLR_CAM_YAW"_hs);
        if (cam_yaw == hub::NOT_FOUND) {
            cam_yaw = pos.yaw;
        }

        float cam_orb = hub::get(registry, owner, "PLR_CAM_ORB"_hs);
        if (cam_orb == hub::NOT_FOUND) {
            cam_orb = 0.0f;
        }

        /**
         * STEP 3: Read terrain height from Hub
         */
        uint32_t pos_hash = static_cast<uint32_t>(
            static_cast<int32_t>(pos.x) * 73856093 ^
            static_cast<int32_t>(pos.z) * 19349663);
        float trn_hgt = hub::get(registry, pos_hash, "TRN_HGT_AT_POS"_hs);
        if (trn_hgt == hub::NOT_FOUND) {
            trn_hgt = 0.0f;
        }

        /**
         * STEP 4: Write to PlayerInpExtComponent (bridge to calc systems)
         */
        auto& inp = registry.get_or_emplace<PlayerInpExtComponent>(entity);
        inp.inp_fwd = inp_fwd;
        inp.inp_str = inp_str;
        inp.inp_sprint = inp_sprint;
        inp.inp_jump = inp_jump;
        inp.cam_yaw = cam_yaw;
        inp.cam_orb = cam_orb;
        inp.trn_hgt = trn_hgt;
    }
}

void PlayerSyncInpSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerSyncInpSystem] Stopped");
}

}  // namespace ase::player
