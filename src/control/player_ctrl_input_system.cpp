/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_ctrl_input_system.cpp
 * @brief       PlayerCtrlInputSystem - Process player input and update facing direction
 * @description SHARED System: Reads from PlayerInpExtComponent (no Hub access).
 *              Calculation systems read from Components, not Hub (SYN Pattern).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    control
 * @schedule    Kinematics
 * @created     2026-01-22
 * @modified    2026-01-22
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_CTRL_INP: Player Input Processing)
 *
 *   [PlayerInpExtComponent from PlayerSyncInpSystem]
 *          │
 *          │ input values from Component (SYN Pattern)
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerCtrlInputSystem         │
 *   │  (SHARED - no Hub access)                   │
 *   │                                             │
 *   │  READS (from Components):                   │
 *   │    - PlayerInpExtComponent (input bridge)   │
 *   │    - PlayerStIdComponent (identity)         │
 *   │    - PlayerStPosComponent (position/yaw)    │
 *   │    - PlayerStMovComponent (turn speed)      │
 *   │                                             │
 *   │  WRITES (to Components):                    │
 *   │    - PlayerStPosComponent (yaw)             │
 *   │    - PlayerStIdComponent (last_input_ms)    │
 *   │    - PlayerDirtyTag (if changed)            │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ player facing updated
 *          ▼
 *   PlayerCtrlMoveSystem (uses facing for movement)
 *
 * SYN Pattern (SHARED Calc System)
 *
 * READS (from PlayerInpExtComponent - filled by PlayerSyncInpSystem):
 *   inp_fwd   → Forward input (-1 to 1)
 *   inp_str   → Strafe input (-1 to 1)
 *   cam_yaw   → Camera yaw (radians)
 *   cam_orb   → Orbit mode flag (0 or 1)
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
 * [ ] hub::get_hub_value() for reads
 * [ ] hub::set_or_create_hub_value() for writes
 * [ ] Method order: on_start → tick → on_stop
 * [ ] ALL THREE METHODS implemented
 * [ ] on_start/on_stop: log::info with system name
 * [ ] log::warn() if value EXISTS but invalid (e.g., health < 0, temp > 1000)
 * [ ] log::error() for EVERY VALUE_NOT_FOUND check (see ase-log/log.hpp ERR::CAT::*)
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
#include <ase/player/systems/control/player_ctrl_input_system.hpp>
// Components from same module ONLY
#include <ase/player/components/input/player_inp_ext_component.hpp>
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Logging
#include <ase/log/log.hpp>
// Math
#include <ase/math/math.hpp>

#include <chrono>

namespace ase::player {

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * IMPORTANT: Use anonymous namespace, NOT static keyword!
 *   ✅ namespace { void helper() {...} }   // CORRECT
 *   ❌ static void helper() {...}          // WRONG!
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

// No helper functions needed - all logic inlined in tick()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerCtrlInputSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerCtrlInputSystem] Started");
}

void PlayerCtrlInputSystem::tick(ecs::Registry& registry, float dt) {
    using namespace std::chrono;

    /**
     * STEP 1: Get turn speed from manager
     */
    float turn_speed = MOVEMENT_DEFAULT_TURN_SPEED;
    auto mov_view = registry.view<PlayerStMovComponent>();
    for (auto [e, mov] : mov_view.each()) {
        (void)e;
        turn_speed = mov.turn_speed;
        break;
    }

    /**
     * STEP 2: Process each player entity
     */
    auto view = registry.view<PlayerStIdComponent, PlayerStPosComponent>();

    for (auto [entity, identity, pos] : view.each()) {
        uint32_t owner = static_cast<uint32_t>(entity);

        /**
         * STEP 3: Read input values from Hub (HUB Pattern - READS)
         */
        float forward = hub::get_hub_value(registry, owner, "PLR_INP_FWD"_hs);
        if (forward == hub::VALUE_NOT_FOUND) {
            forward = 0.0f;
        }

        float strafe = hub::get_hub_value(registry, owner, "PLR_INP_STR"_hs);
        if (strafe == hub::VALUE_NOT_FOUND) {
            strafe = 0.0f;
        }

        float cam_yaw = hub::get_hub_value(registry, owner, "PLR_CAM_YAW"_hs);
        if (cam_yaw == hub::VALUE_NOT_FOUND) {
            cam_yaw = pos.yaw;
        }

        float orbit_mode = hub::get_hub_value(registry, owner, "PLR_CAM_ORB"_hs);
        if (orbit_mode == hub::VALUE_NOT_FOUND) {
            orbit_mode = 0.0f;
        }

        bool is_moving = (forward != 0.0f || strafe != 0.0f);
        bool is_orbit = (orbit_mode > 0.5f);

        /**
         * STEP 4: Update player facing based on camera yaw (if moving and not in orbit mode)
         */
        if (is_moving && !is_orbit) {
            float delta = cam_yaw - pos.yaw;
            while (delta > math::PI) delta -= math::TWO_PI;
            while (delta < -math::PI) delta += math::TWO_PI;

            float max_turn = turn_speed * dt;
            if (math::abs(delta) < max_turn) {
                pos.yaw = cam_yaw;
            } else {
                pos.yaw += (delta > 0.0f ? max_turn : -max_turn);
            }

            while (pos.yaw < 0.0f) pos.yaw += math::TWO_PI;
            while (pos.yaw >= math::TWO_PI) pos.yaw -= math::TWO_PI;

            registry.emplace_or_replace<PlayerDirtyTag>(entity);
        }

        /**
         * STEP 5: Update last input timestamp
         */
        identity.last_input_ms = static_cast<uint64_t>(
            duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
    }
}

void PlayerCtrlInputSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerCtrlInputSystem] Stopped");
}

}  // namespace ase::player
