/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_pos_sys.cpp
 * @brief       PlayerHubPosSystem - Write player positions to Hub for cross-module access
 * @description Publishes the world-metre view of every player to the Hub for L4 plugins and other modules.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:100049 - publish a hub value
 * @actor       Hub Value Bridge (HUBVAL)
 * @category    hub
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-10-12
 * @version     1.2.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_HUB_POS: Player Position Hub Publishing)
 *
 *   [PlayerStaWrldComponent - formed by PlayerSimWrldSystem]
 *          │
 *          │ world metres in the axis order of the star (x, y horizontal, z height)
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubPosSystem            │
 *   │                                             │
 *   │  READS:                                     │
 *   │    → PlayerStaIdntComponent (identity)      │
 *   │    → PlayerStaWrldComponent (world view)    │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    → "PLR_POS_X"_hs (Hub)                   │
 *   │    → "PLR_POS_Y"_hs (Hub)                   │
 *   │    → "PLR_POS_Z"_hs (Hub)                   │
 *   │    → "PLR_ENTITY_ID"_hs (Hub)               │
 *   │    → "PLR_IS_PLAYER"_hs (Hub)               │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ position available via Hub
 *          ▼
 *   L4 Plugins (read via sdk::get)
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1)):
 *
 * READS (from player module via Components):
 *   PlayerStaIdntComponent  → Player identity for iteration
 *   PlayerStaWrldComponent  → World-metre view (x, y horizontal, z height)
 *
 * WRITES (to Hub for other modules):
 *   "PLR_POS_X"_hs       → first horizontal axis, world metres (float)
 *   "PLR_POS_Y"_hs       → second horizontal axis, world metres - never the height (float)
 *   "PLR_POS_Z"_hs       → height axis, metres (float)
 *   "PLR_ENTITY_ID"_hs   → Entity ID as float for reference
 *   "PLR_IS_PLAYER"_hs   → 1.0f marker for player entities
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
#include <ase/player/systems/hub/player_hub_pos_sys.hpp>
// Components from same module
#include <ase/player/components/state/player_sta_wrld_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
// Hub for O(1) API
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

// No helper functions needed - this system is simple enough for inline processing

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerHubPosSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubPosSystem] Started");
}

void PlayerHubPosSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Create view and iterate player entities
     * Views are created on demand, not stored as member variables.
     * PlayerStaIdntComponent identifies player entities; PlayerStaWrldComponent is the view
     * PlayerSimWrldSystem formed from the settled place in this frame.
     */
    auto view = registry.view<PlayerStaIdntComponent, PlayerStaWrldComponent>();

    for (auto [entity, id, wrld] : view.each()) {
        uint32_t owner = static_cast<uint32_t>(entity);

        /**
         * STEP 2: Write position to Hub (HUB Pattern - WRITES)
         * Publish position data for L4 plugins to consume via sdk::hub::get().
         * This enables loose coupling - plugins read from Hub, not direct components.
         *
         * THE THREE FIELDS ARE COPIED, NEVER SWAPPED OR SUMMED HERE. The view already speaks the
         * axis order hub_metrics.json declares: PLR_POS_Y is the second horizontal axis, never the
         * height, and PLR_POS_Z is the height. A band derived from these axes is silently wrong if
         * the two disagree, and no test reports it; the swap therefore lives in one place, in
         * PlayerSimWrldSystem, the same way ase-spatial keeps it in SpatialIdxWrldSystem, so a
         * consumer that pours PLR_POS_* and SPT_POS_* into the same three fields gets the same for
         * both kinds of body.
         */
        hub::set(registry, owner, "PLR_POS_X"_hs, wrld.x);
        hub::set(registry, owner, "PLR_POS_Y"_hs, wrld.y);
        hub::set(registry, owner, "PLR_POS_Z"_hs, wrld.z);
        hub::set(registry, owner, "PLR_ENTITY_ID"_hs, static_cast<float>(owner));
        hub::set(registry, owner, "PLR_IS_PLAYER"_hs, 1.0f);
    }
}

void PlayerHubPosSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubPosSystem] Stopped");
}

}  // namespace ase::player
