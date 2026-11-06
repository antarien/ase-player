/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_ctrl_mov_sys.cpp
 * @brief       PlayerCtrlMovSystem - Calculate player velocity from input
 * @description Calculation system of the SYN pattern: reads input, camera and the movement restriction
 *              of the combat star from bridge components, never from the Hub. The restriction row
 *              PlayerInpCmbComponent is server_only, so the system is server_only with it.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    action/control
 * @schedule    Dynamics
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-01-22
 * @modified    2026-11-06
 * @version     1.2.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_CTRL_MOV: Player Movement Calculation)
 *
 *   [PlayerInpExtComponent from PlayerSyncInpSystem, PlayerInpCmbComponent from PlayerCmbSynSystem]
 *          │
 *          │ input values and combat restriction from Components (SYN Pattern)
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerCtrlMovSystem           │
 *   │  (no Hub access)                            │
 *   │                                             │
 *   │  READS (from Components):                   │
 *   │    - PlayerInpExtComponent (input bridge)   │
 *   │    - PlayerInpCamComponent (camera yaw)     │
 *   │    - PlayerInpCmbComponent (slow, stun)     │
 *   │    - PlayerStaPhysComponent (on_ground)     │
 *   │                                             │
 *   │  WRITES (to Components):                    │
 *   │    - PlayerStaVelComponent (vx, vy, vz)     │
 *   │    - PlayerStaPhysComponent (on_ground)     │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ velocity calculated
 *          ▼
 *   PlayerSimKnbkSystem (knockback), then PlayerSimPhysSystem (applies velocity to position)
 *
 * COMBAT RESTRICTION: slow and frost each take their fraction of the target speed, multiplied with
 * each other; a running landing stun or blunder recovery sets the target speed to 0. Gravity and the
 * jump are untouched.
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   (none - uses SYN pattern, reads from PlayerInpExtComponent and PlayerInpCmbComponent)
 *
 * WRITES (to Hub for other modules):
 *   (none - uses SYN pattern, writes to Components only)
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
// FORBIDDEN: <vector>, <map>, <unordered_map>, <optional>, <algorithm>, <chrono>
// ALLOWED:   <cstdint>, <cmath>, <cassert>, ase-* headers

// Own header FIRST
#include <ase/player/systems/control/player_ctrl_mov_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/input/player_inp_ext_comp.hpp>
#include <ase/player/components/input/player_inp_cam_comp.hpp>
#include <ase/player/components/input/player_inp_cmb_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_phys_comp.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// ase-types (Layer 0) for the positive-float predicate of the stuns
#include <ase/types/types.hpp>
// Logging
#include <ase/log/log.hpp>
// Math
#include <ase/math/math.hpp>

namespace ase::player {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

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

void PlayerCtrlMovSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerCtrlMovSystem] Started");
}

void PlayerCtrlMovSystem::tick(ecs::Registry& registry, float dt) {
    /**
     * STEP 1: The movement settings are the MOVEMENT_DEFAULT_* constants of types.hpp.
     */

    /**
     * STEP 2: Process each player entity with input data (SYN Pattern)
     * Reads from PlayerInpExtComponent (filled by PlayerSyncInpSystem) and PlayerInpCmbComponent
     * (filled by PlayerCmbSynSystem for the same figures).
     */
    auto view = registry.view<
        PlayerInpExtComponent,
        PlayerInpCamComponent,
        PlayerInpCmbComponent,
        PlayerStaPosComponent,
        PlayerStaVelComponent,
        PlayerStaPhysComponent
    >();

    for (auto [entity, inp, cam, cmb, pos, vel, physics] : view.each()) {
        /**
         * STEP 3: Read input values from the bridge components (SYN Pattern).
         * The axes live in PlayerInpExtComponent, the camera yaw in PlayerInpCamComponent.
         */
        float forward = inp.inp_fwd;
        float strafe = inp.inp_str;
        bool sprint = (inp.inp_sprint > 0.5f);
        bool jump = (inp.inp_jump > 0.5f);
        float movement_yaw = cam.cam_yaw;

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
         * STEP 5: Calculate target velocity, restricted by the combat star.
         * Slow and frost each take their fraction; a running stun holds the figure in place.
         */
        float speed = sprint ? MOVEMENT_DEFAULT_RUN_SPEED : MOVEMENT_DEFAULT_WALK_SPEED;
        speed *= (PLR_CMB_SCALE_FULL - cmb.slow_intensity) * (PLR_CMB_SCALE_FULL - cmb.freeze_slow);
        if (ase::types::is_pos_float(cmb.landing_stun) || ase::types::is_pos_float(cmb.patzer_stun)) {
            speed = PLR_CMB_UNIT_MIN;
        }
        float control = physics.on_ground ? 1.0f : MOVEMENT_DEFAULT_AIR_CONTROL;

        float target_vx = move_x * speed;
        float target_vz = move_z * speed;

        /**
         * STEP 6: Apply acceleration based on ground state
         */
        if (physics.on_ground) {
            float accel = MOVEMENT_DEFAULT_GROUND_FRICTION * dt;
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
            vel.vy = MOVEMENT_DEFAULT_JUMP_IMPULSE;
            physics.on_ground = false;
        }

        /**
         * STEP 8: Apply gravity
         */
        if (!physics.on_ground && physics.gravity_enabled) {
            vel.vy -= MOVEMENT_DEFAULT_GRAVITY * dt;
        }

        (void)pos;  // Position read for potential future use
        (void)entity;  // Entity available for potential tagging
    }
}

void PlayerCtrlMovSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerCtrlMovSystem] Stopped");
}

}  // namespace ase::player
