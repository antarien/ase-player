/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sim_wrld_sys.cpp
 * @brief       PlayerSimWrldSystem - turns the chunk-relative place of every player figure into world metres
 * @description Pure conversion of the settled place into the world-metre view the hub publisher copies.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:100384 - convert place to world metres
 * @actor       Scene Graph Optimizations (SGO)
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-10-12
 * @modified    2026-10-12
 * @version     1.0.0
 *
 * CAUSAL CHAIN (DSGN_018 SGO//SPSUB: where does this player stand in world metres?)
 *
 *   [the place has settled for this frame: PlayerStaPosComponent]
 *          │
 *          ▼
 *   ┌─────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSimWrldSystem                       │
 *   │                                                         │
 *   │  READS:  PlayerStaPosComponent                          │
 *   │  WRITES: PlayerStaWrldComponent                         │
 *   └─────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   PlayerHubPosSystem copies the view onto PLR_POS_X/Y/Z.
 *
 * ONE SUM PER HORIZONTAL AXIS: chunk times chunk edge plus local metres. At a chunk edge the sum is exact,
 * because multiples of the edge below 2^28 lie on mantissa bits; its rounding touches only the published view,
 * never the path - the physics step keeps moving the chunk-relative place, whose local rest stays small at the
 * outer edge of the address space.
 *
 * THE AXIS ORDER OF THE DECLARATION (hub_metrics.json): x is the first horizontal axis, y the second, z the
 * height. This module carries the height in PlayerStaPosComponent.y and spans the chunk grid with x and z;
 * whoever carries values between the module and the star swaps them, and the swap happens here and nowhere
 * else. ase-spatial swaps the same way (SPT_POS_Y the second horizontal axis, SPT_POS_Z the height), so a
 * consumer that pours PLR_POS_* and SPT_POS_* into the same three fields gets the same for both kinds of body.
 *
 * HUB Pattern (ARCH_ASE_HUB_API v2.0)
 *
 * READS (from Hub):   none
 * WRITES (to Hub):    none - PlayerHubPosSystem publishes the view
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
#include <ase/player/systems/simulation/player_sim_wrld_sys.hpp>
// Components from same module
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_wrld_comp.hpp>
// types.hpp for constants (chunk edge for the world-metre sum)
#include <ase/player/types.hpp>
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

// No helpers - the view is one sum per horizontal axis in the loop that writes it

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerSimWrldSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimWrldSystem] Started");
}

void PlayerSimWrldSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Every place of the frame gets its world-metre view. The view carries no type the loop writes,
     * so placing the component on a figure that has none yet does not change the set being walked.
     */
    for (auto [entity, pos] : registry.view<PlayerStaPosComponent>().each()) {
        auto& wrld = registry.emplace_or_replace<PlayerStaWrldComponent>(entity);
        wrld.x = static_cast<float>(pos.chunk_x) * MOVEMENT_DEFAULT_CHUNK_SIZE + pos.local_x;
        wrld.y = static_cast<float>(pos.chunk_z) * MOVEMENT_DEFAULT_CHUNK_SIZE + pos.local_z;
        wrld.z = pos.y;
    }
}

void PlayerSimWrldSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSimWrldSystem] Stopped");
}

}  // namespace ase::player
