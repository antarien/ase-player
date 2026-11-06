/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sim_phys_sys.cpp
 * @brief       PlayerSimPhysSystem - Apply physics simulation to player entities
 * @description SHARED System: Reads from PlayerInpExtComponent (no Hub access).
 *              Calculation systems read from Components, not Hub (SYN Pattern).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    process/simulation
 * @schedule    Dynamics
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-01-22
 * @modified    2026-11-01
 * @version     1.2.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_SIM_PHYS: Player Physics Simulation)
 *
 *   [PlayerStaVelComponent from PlayerCtrlMovSystem]
 *          │
 *          │ velocity calculated
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSimPhysSystem           │
 *   │  (SHARED - no Hub access)                   │
 *   │                                             │
 *   │  READS (from Components):                   │
 *   │    - PlayerInpTrnComponent (trn_hgt)        │
 *   │    - PlayerInpChnkCnstComponent (edge)      │
 *   │    - PlayerStaPosComponent (position)       │
 *   │    - PlayerStaVelComponent (velocity)       │
 *   │    - PlayerStaPhysComponent (physics state) │
 *   │                                             │
 *   │  WRITES (to Components):                    │
 *   │    - PlayerStaPosComponent (x, y, z)        │
 *   │    - PlayerStaVelComponent (vy on ground)   │
 *   │    - PlayerStaPhysComponent (on_ground)     │
 *   │    - PlayerDrtyTag (if moving)              │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ position updated
 *          ▼
 *   PlayerStaStsSystem (updates player status)
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   (none - uses SYN pattern, reads terrain height from PlayerInpTrnComponent and the chunk edge
 *   TER_CONST_CHUNK_SIZE from PlayerInpChnkCnstComponent on the manager row)
 *
 * WRITES (to Hub for other modules):
 *   (none - uses SYN pattern, writes to Components only)
 *
 * NOTE: This is a SHARED calculation system using the SYN pattern.
 * Terrain height is filled from the Hub into PlayerInpTrnComponent by PlayerSyncInpSystem, the
 * chunk edge into PlayerInpChnkCnstComponent by PlayerChnkCnstSynSystem, before this system runs.
 * This system operates on Component data only.
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
#include <ase/player/systems/simulation/player_sim_phys_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/input/player_inp_trn_comp.hpp>
#include <ase/player/components/input/player_inp_chnk_cnst_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_phys_comp.hpp>
#include <ase/player/components/tag/player_drty_tag.hpp>
#include <ase/player/components/tag/player_mgr_tag.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
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

/**
 * @brief Add one step in metres to the chunk-relative track, carry over into the chunk address
 *
 * S2b 2026-08-11 (the same cut as P22 in bdi_act_mov_sys.cpp): the step ALWAYS lands on the
 * small local metres with full float precision - `pos += vel*dt` on absolute world metres
 * swallowed every 0.04 m step at 204 million metres (place 7, ULP 16 m), and the settlers of
 * that place stood still (measured live 2026-08-11 14:09).
 *
 * @param chunk    Chunk address of the axis (exact, carried on when the edge is crossed)
 * @param local    Metres inside the chunk [0, edge)
 * @param delta_m  Step in metres
 * @param edge     Chunk edge in metres (PlayerInpChnkCnstComponent.chunk_size)
 */
void advance_local(int32_t& chunk, float& local, float delta_m, float edge) {
    local += delta_m;
    while (local >= edge) { local -= edge; ++chunk; }
    while (local < 0.0f)  { local += edge; --chunk; }
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerSimPhysSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimPhysSystem] Started");
}

void PlayerSimPhysSystem::tick(ecs::Registry& registry, float dt) {
    /**
     * STEP 1: Physics settings. Snap distance and velocity epsilon are constants of this module;
     * the chunk edge is the terrain protocol's TER_CONST_CHUNK_SIZE, laid on the manager row by
     * PlayerChnkCnstSynSystem. Without it no figure moves this tick: a step carried over on a
     * guessed edge would put it into the wrong chunk.
     */
    float ground_snap = MOVEMENT_DEFAULT_GROUND_SNAP_DIST;
    float vel_eps = MOVEMENT_DEFAULT_VELOCITY_EPSILON;
    float chunk_edge = 0.0f;
    bool edge_loaded = false;
    for (auto [mgr, edge] : registry.view<PlayerMgrTag, PlayerInpChnkCnstComponent>().each()) {
        (void)mgr;
        chunk_edge = edge.chunk_size;
        edge_loaded = true;
    }
    if (!edge_loaded) {
        return;
    }

    /**
     * STEP 2: Process each player entity with input data (SYN Pattern)
     * Reads terrain height from PlayerInpTrnComponent (filled by PlayerSyncInpSystem)
     */
    auto view = registry.view<
        PlayerInpTrnComponent,
        PlayerStaPosComponent,
        PlayerStaVelComponent,
        PlayerStaPhysComponent
    >();

    for (auto [entity, trn, pos, vel, physics] : view.each()) {
        /**
         * STEP 3: Apply velocity to position (chunk-relative, S2b 2026-08-11)
         * The plane runs over the local track and never fades; the vertical stays small metres
         * and may integrate directly.
         */
        advance_local(pos.chunk_x, pos.local_x, vel.vx * dt, chunk_edge);
        advance_local(pos.chunk_z, pos.local_z, vel.vz * dt, chunk_edge);
        pos.y += vel.vy * dt;

        /**
         * STEP 4: Get terrain height from PlayerInpTrnComponent (SYN Pattern)
         * Terrain height was read from the Hub by PlayerSyncInpSystem
         */
        float ground_height = trn.trn_hgt;

        /**
         * STEP 5: Ground collision detection and response
         */
        if (pos.y <= ground_height + ground_snap) {
            pos.y = ground_height;
            vel.vy = 0.0f;
            physics.on_ground = true;
        } else {
            physics.on_ground = false;
        }

        /**
         * STEP 6: Mark dirty if moving (using ase-math sqrt)
         */
        float vel_len = math::sqrt(vel.vx * vel.vx + vel.vz * vel.vz);
        if (vel_len > vel_eps) {
            registry.emplace_or_replace<PlayerDrtyTag>(entity);
        }
    }
}

void PlayerSimPhysSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimPhysSystem] Stopped");
}

}  // namespace ase::player
