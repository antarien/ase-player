/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_ctrl_inp_sys.cpp
 * @brief       PlayerCtrlInpSystem - Process player input and update facing direction
 * @description SHARED System: Reads from PlayerInpExtComponent (no Hub access).
 *              Calculation systems read from Components, not Hub (SYN Pattern).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    action/control
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_CTRL_INP: Player Input Processing)
 *
 *   [PlayerInpExtComponent from PlayerSyncInpSystem]
 *          │
 *          │ input values from Component (SYN Pattern)
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerCtrlInpSystem           │
 *   │  (SHARED - no Hub access)                   │
 *   │                                             │
 *   │  READS (from Components):                   │
 *   │    - PlayerInpExtComponent (input bridge)   │
 *   │    - PlayerStaIdntComponent (identity)         │
 *   │    - PlayerStaPosComponent (position/yaw)    │
 *   │    - PlayerStMovComponent (turn speed)      │
 *   │                                             │
 *   │  WRITES (to Components):                    │
 *   │    - PlayerStaPosComponent (yaw)             │
 *   │    - PlayerDrtyTag (if changed)            │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ player facing updated
 *          ▼
 *   PlayerCtrlMovSystem (uses facing for movement)
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   (none - uses SYN pattern, reads from PlayerInpExtComponent)
 *
 * WRITES (to Hub for other modules):
 *   (none - uses SYN pattern, writes to Components only)
 *
 * NOTE: This is a SHARED calculation system using the SYN pattern.
 * Input data is synced from Hub to PlayerInpExtComponent by PlayerSyncInpSystem
 * before this system runs. This system operates on Component data only.
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
#include <ase/player/systems/control/player_ctrl_inp_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/input/player_inp_ext_comp.hpp>
#include <ase/player/components/input/player_inp_cam_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/state/player_sta_yaw_comp.hpp>
#include <ase/player/components/tag/player_drty_tag.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Logging
#include <ase/log/log.hpp>
// Math
#include <ase/math/math.hpp>
// Sentinel and range predicates (NO manual sentinel comparisons!)
#include <ase/types/types.hpp>

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

void PlayerCtrlInpSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerCtrlInpSystem] Started");
}

void PlayerCtrlInpSystem::tick(ecs::Registry& registry, float dt) {
    /**
     * STEP 1: Get turn speed from manager
     */
    float turn_speed = MOVEMENT_DEFAULT_TURN_SPEED;

    /**
     * STEP 2: Process each player entity with input data (SYN Pattern)
     * Reads from PlayerInpExtComponent (filled by PlayerSyncInpSystem)
     */
    auto view = registry.view<PlayerStaIdntComponent, PlayerStaYawComponent,
                              PlayerInpExtComponent, PlayerInpCamComponent>();

    for (auto [entity, identity, yaw, inp, cam] : view.each()) {
        (void)identity;

        /**
         * STEP 3: Read input values from the bridge components (SYN Pattern).
         * The axes live in PlayerInpExtComponent, the camera values in
         * PlayerInpCamComponent - split 2026-08-15 along the consumption.
         */
        float forward = inp.inp_fwd;
        float strafe = inp.inp_str;
        float cam_yaw = cam.cam_yaw;
        float orbit_mode = cam.cam_orb;

        bool is_moving = (forward != 0.0f || strafe != 0.0f);
        bool is_orbit = (orbit_mode > 0.5f);

        /**
         * STEP 4: Update player facing based on camera yaw (if moving and not in orbit mode)
         */
        if (is_moving && !is_orbit) {
            float delta = cam_yaw - yaw.yaw;
            while (delta > math::PI) delta -= math::TWO_PI;
            while (delta < -math::PI) delta += math::TWO_PI;

            float max_turn = turn_speed * dt;
            if (math::abs(delta) < max_turn) {
                yaw.yaw = cam_yaw;
            } else {
                /* `is_pos_float` statt `> 0.0f`: FloatUnset (FLT_MAX) ist AUCH groesser als null.
                 * Die Drehrichtung ist hier die einzige Aussage des Ausdrucks - kaeme aus
                 * cam.cam_yaw ein nie gesetzter Wert, drehte der Spieler mit voller Rate in die
                 * Richtung, die der Sentinel diktiert, statt in die gemessene. */
                yaw.yaw += (types::is_pos_float(delta) ? max_turn : -max_turn);
            }

            while (yaw.yaw < 0.0f) yaw.yaw += math::TWO_PI;
            while (yaw.yaw >= math::TWO_PI) yaw.yaw -= math::TWO_PI;

            registry.emplace_or_replace<PlayerDrtyTag>(entity);
        }
    }
}

void PlayerCtrlInpSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerCtrlInpSystem] Stopped");
}

}  // namespace ase::player
