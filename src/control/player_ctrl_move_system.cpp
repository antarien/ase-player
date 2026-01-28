/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_ctrl_move_system.cpp
 * @brief       PlayerCtrlMoveSystem - Calculate player velocity from input
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
 * CAUSAL CHAIN (CAUSA_PLR_CTRL_MOV: Player Movement Calculation)
 *
 *   [PlayerInpExtComponent from PlayerSyncInpSystem]
 *          │
 *          │ input values from Component (SYN Pattern)
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerCtrlMoveSystem          │
 *   │  (SHARED - no Hub access)                   │
 *   │                                             │
 *   │  READS (from Components):                   │
 *   │    - PlayerInpExtComponent (input bridge)   │
 *   │    - PlayerStPosComponent (yaw)             │
 *   │    - PlayerStVelComponent (current vel)     │
 *   │    - PlayerStPhysComponent (on_ground)      │
 *   │    - PlayerStMovComponent (speed settings)  │
 *   │                                             │
 *   │  WRITES (to Components):                    │
 *   │    - PlayerStVelComponent (vx, vy, vz)      │
 *   │    - PlayerStPhysComponent (on_ground)      │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ velocity calculated
 *          ▼
 *   PlayerSimPhysSystem (applies velocity to position)
 *
 * SYN Pattern (SHARED Calc System)
 *
 * READS (from PlayerInpExtComponent - filled by PlayerSyncInpSystem):
 *   inp_fwd     → Forward input (-1 to 1)
 *   inp_str     → Strafe input (-1 to 1)
 *   inp_sprint  → Sprint input (0 or 1)
 *   inp_jump    → Jump input (0 or 1)
 *   cam_yaw     → Camera yaw (radians)
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
#include <ase/player/systems/control/player_ctrl_move_system.hpp>
// Components from same module ONLY
#include <ase/player/components/input/player_inp_ext_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_phys_component.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Logging
#include <ase/log/log.hpp>
// Math
#include <ase/math/math.hpp>

namespace ase::player {
using namespace entt::literals;  // For "_hs hashed strings

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

void PlayerCtrlMoveSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerCtrlMoveSystem] Started");
}

void PlayerCtrlMoveSystem::tick(ecs::Registry& registry, float dt) {
    /**
     * STEP 1: Get movement settings from manager
     */
    PlayerStMovComponent mov;
    mov.walk_speed = MOVEMENT_DEFAULT_WALK_SPEED;
    mov.run_speed = MOVEMENT_DEFAULT_RUN_SPEED;
    mov.jump_impulse = MOVEMENT_DEFAULT_JUMP_IMPULSE;
    mov.gravity = MOVEMENT_DEFAULT_GRAVITY;
    mov.ground_friction = MOVEMENT_DEFAULT_GROUND_FRICTION;
    mov.air_control = MOVEMENT_DEFAULT_AIR_CONTROL;

    auto mov_view = registry.view<PlayerStMovComponent>();
    for (auto [e, m] : mov_view.each()) {
        (void)e;
        mov = m;
        break;
    }

    /**
     * STEP 2: Process each player entity with input data (SYN Pattern)
     * Reads from PlayerInpExtComponent (filled by PlayerSyncInpSystem)
     */
    auto view = registry.view<
        PlayerInpExtComponent,
        PlayerStPosComponent,
        PlayerStVelComponent,
        PlayerStPhysComponent
    >();

    for (auto [entity, inp, pos, vel, physics] : view.each()) {
        /**
         * STEP 3: Read input values from PlayerInpExtComponent (SYN Pattern)
         */
        float forward = inp.inp_fwd;
        float strafe = inp.inp_str;
        bool sprint = (inp.inp_sprint > 0.5f);
        bool jump = (inp.inp_jump > 0.5f);
        float movement_yaw = inp.cam_yaw;

        /**
         * STEP 4: Calculate movement direction using ase-math
         */
        float sin_yaw = math::sin(movement_yaw);
        float cos_yaw = math::cos(movement_yaw);

        float move_x = -forward * sin_yaw + strafe * cos_yaw;
        float move_z = -forward * cos_yaw - strafe * sin_yaw;

        float move_len = math::sqrt(move_x * move_x + move_z * move_z);
        if (move_len > 1.0f) {
            move_x /= move_len;
            move_z /= move_len;
        }

        /**
         * STEP 5: Calculate target velocity
         */
        float speed = sprint ? mov.run_speed : mov.walk_speed;
        float control = physics.on_ground ? 1.0f : mov.air_control;

        float target_vx = move_x * speed;
        float target_vz = move_z * speed;

        /**
         * STEP 6: Apply acceleration based on ground state
         */
        if (physics.on_ground) {
            float accel = mov.ground_friction * dt;
            accel = math::min(accel, 1.0f);
            vel.vx += (target_vx - vel.vx) * accel;
            vel.vz += (target_vz - vel.vz) * accel;
        } else {
            vel.vx += (target_vx - vel.vx) * control * dt;
            vel.vz += (target_vz - vel.vz) * control * dt;
        }

        /**
         * STEP 7: Handle jump
         */
        if (jump && physics.on_ground) {
            vel.vy = mov.jump_impulse;
            physics.on_ground = false;
        }

        /**
         * STEP 8: Apply gravity
         */
        if (!physics.on_ground && physics.gravity_enabled) {
            vel.vy -= mov.gravity * dt;
        }

        (void)pos;  // Position read for potential future use
        (void)entity;  // Entity available for potential tagging
    }
}

void PlayerCtrlMoveSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerCtrlMoveSystem] Stopped");
}

}  // namespace ase::player
