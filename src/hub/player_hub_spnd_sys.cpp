/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_spnd_sys.cpp
 * @brief       PlayerHubSpndSystem - publishes every figure in the world as a HUB_PLR_SPND row
 * @description Adopts the hub row kind HUB_PLR_SPND onto every entity that carries PlayerSpndTag.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:5130 - play a character
 * @actor       SS/PC
 * @category    hub
 * @schedule    Dynamics
 * @created     2026-10-13
 * @modified    2026-10-13
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_HUB_SPND: the figure roster of the star)
 *
 *   [PlayerSpndTag - set by PlayerLifeSpwnSystem on a spawned figure and by PlayerMigDesSystem
 *    on a figure that arrived from another region]
 *          │
 *          │ the figure stands in the world
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubSpndSystem           │
 *   │                                             │
 *   │  READS:                                     │
 *   │    → PlayerSpndTag (the figures)            │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    → hub row kind HUB_PLR_SPND (adopt)      │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ the roster is a row kind of the hub
 *          ▼
 *   ase-bdi, ase-camera, ase-character, ase-geoid and ase-world walk or test the row
 *
 * WHY A SYSTEM OF ITS OWN. The spawn and the migration compute the figure - its chunk address,
 * its local metres - and a hub write in the same file is hub I/O beside calculation
 * (HUB_IO_MIXED_WITH_MATH, WRFL_ASE_SYN_PATTERN). They state the fact on their own tag; this
 * system carries it to the star and computes nothing.
 *
 * THE ROW ENDS WITH THE FIGURE. Nobody withdraws it: when the figure entity is destroyed, the
 * storage of the kind drops it with it. The kind has no field (hub_rows.json, HUB_PLR_SPND).
 *
 * HUB Pattern (ARCH_ASE_HUB_API, Typisierte Zeilen):
 *
 * READS (from player module via Components):
 *   PlayerSpndTag → the figures in the world
 *
 * WRITES (to Hub for other modules):
 *   row kind HUB_PLR_SPND → hub::row_adopt onto the figure entity, idempotent
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
#include <ase/player/systems/hub/player_hub_spnd_sys.hpp>
// Components from same module
#include <ase/player/components/tag/player_spnd_tag.hpp>
// Hub for the row API
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

// No helper functions needed - one adoption per figure

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerHubSpndSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubSpndSystem] Started");
}

void PlayerHubSpndSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * EVERY FIGURE IS ADOPTED, EVERY PASS. hub::row_adopt is idempotent - a figure that is a row
     * already stays one - so the pass needs no test against the kind's storage and no second
     * marker. The kind lives in a storage of its own; adopting into it does not change the set
     * this view walks.
     */
    for (auto figure : registry.view<PlayerSpndTag>()) {
        hub::row_adopt(registry, figure, "HUB_PLR_SPND"_hs);
    }
}

void PlayerHubSpndSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubSpndSystem] Stopped");
}

}  // namespace ase::player
