/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sim_tlpt_sys.cpp
 * @brief       PlayerSimTlptSystem - Applies the backend teleport jump to the player position
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-10-03
 * @modified    2026-10-03
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_SIM_TLPT: bridged teleport lever → realised position jump)
 *
 *   [PlayerInpTlptComponent.jump_m — mirrored from PLR_CHEAT_TELEPORT by PlayerSyncTlptSystem]
 *          │
 *          ▼
 *   ┌──────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSimTlptSystem (calc, no Hub)          │
 *   │                                                          │
 *   │  READS:                                                  │
 *   │    → PlayerInpTlptComponent.jump_m                       │
 *   │                                                          │
 *   │  WRITES:                                                 │
 *   │    → PlayerStaPosComponent.local_x (+ jump)              │
 *   └──────────────────────────────────────────────────────────┘
 *          │
 *          │ PlayerSimPhysSystem carries the overflow across the cell edge in the same tick
 *          ▼
 *   PlayerAccMovSystem: per-tick delta above PLR_AC_TELEPORT_STEP → PLAYER_MOVEMENT_SUSPICIOUS
 *
 * WHY THE POSITION AND NOT THE VELOCITY
 *
 * PlayerSimChtSystem forces the velocity, which the speed authority catches; a teleport is a jump
 * the velocity never shows. Inducing it through the velocity would prove the speed check twice and
 * the teleport check never. The jump therefore lands on local_x alone, and the velocity of the
 * player stays what the movement authority gave it.
 *
 * WHY BEFORE THE PHYSICS STEP
 *
 * local_x lives in [0, edge). The jump may leave that range; PlayerSimPhysSystem, ordered after
 * this system, adds its own step and then carries every overflow into chunk_x. No reader in the
 * tick sees an unnormalised coordinate, and the carry has one implementation, not two. The view
 * selects exactly the components the physics step requires, so no jumped player escapes it.
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1)):
 *
 * READS (from Hub):
 *   (none — the lever arrives over PlayerInpTlptComponent, SYN pattern)
 *
 * WRITES (to Hub):
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
#include <ase/player/systems/simulation/player_sim_tlpt_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/input/player_inp_tlpt_comp.hpp>
// The four components PlayerSimPhysSystem integrates — the view selects exactly those players
#include <ase/player/components/input/player_inp_trn_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_phys_comp.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// ase-types (Layer 0) for the positive-float SSOT predicate
#include <ase/types/types.hpp>
// Logging
#include <ase/log/log.hpp>

#include <entt/core/hashed_string.hpp>

#include <cstdint>

namespace ase::player {
using namespace entt::literals;

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

// No helper functions needed - one addition per jumped player; the cell-edge carry is the physics
// step's, ordered after this system.

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerSimTlptSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimTlptSystem] Started");
}

void PlayerSimTlptSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto view = registry.view<PlayerInpTlptComponent, PlayerInpTrnComponent, PlayerStaPosComponent,
                              PlayerStaVelComponent, PlayerStaPhysComponent>();

    for (auto [entity, inp, trn, pos, vel, phys] : view.each()) {
        // Selection only: these three make this view the physics step's view, so every jump is
        // carried across the cell edge in the same tick.
        (void)entity;
        (void)trn;
        (void)vel;
        (void)phys;
        if (!ase::types::is_pos_float(inp.jump_m)) {
            continue;  // lever cleared: no jump this tick
        }
        pos.local_x += inp.jump_m;
    }
}

void PlayerSimTlptSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimTlptSystem] Stopped");
}

}  // namespace ase::player
