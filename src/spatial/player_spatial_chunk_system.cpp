/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_spatial_chunk_system.cpp
 * @brief       PlayerSpatialChunkSystem - Track player chunk position for streaming
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    spatial
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-22
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_SPA_CHK: Player Chunk Tracking)
 *
 *   [PlayerStPosComponent from PlayerSimPhysSystem]
 *          │
 *          │ position updated
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSpatialChunkSystem      │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - PlayerStPosComponent (x, z)            │
 *   │    - PlayerStChkComponent (current chunk)   │
 *   │    - PlayerStMovComponent (chunk_size)      │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - PlayerStChkComponent (chunk_x, chunk_y)│
 *   │    - PlayerChunkChangedTag (if changed)     │
 *   │    - "PLR_CHK_CHG"_hs (Hub - notify terrain)│
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ chunk changed notification
 *          ▼
 *   TerrainStreamingSystem (via Hub)
 *
 * HUB Pattern (MIG_ASE_HUB)
 *
 * READS (from Components):
 *   PlayerStPosComponent → Current position
 *   PlayerStChkComponent → Current chunk
 *
 * WRITES (to Hub for other modules):
 *   "PLR_CHK_CHG"_hs → Player chunk changed notification
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
#include <ase/player/systems/spatial/player_spatial_chunk_system.hpp>
// Components from same module ONLY
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_chk_component.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>
#include <ase/player/components/tag/player_tag_chunk_changed_component.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Hub for HUB Pattern
#include <ase/hub/hub.hpp>
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

void PlayerSpatialChunkSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerSpatialChunkSystem] Started");
}

void PlayerSpatialChunkSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Get chunk size from manager
     */
    float chunk_size = MOVEMENT_DEFAULT_CHUNK_SIZE;

    auto mov_view = registry.view<PlayerStMovComponent>();
    for (auto [e, mov] : mov_view.each()) {
        (void)e;
        chunk_size = mov.chunk_size;
        break;
    }

    /**
     * STEP 2: Process each player entity
     */
    auto view = registry.view<PlayerStPosComponent, PlayerStChkComponent>();

    for (auto [entity, pos, chunk] : view.each()) {
        /**
         * STEP 3: Calculate new chunk coordinates using ase-math floor
         */
        int32_t new_chunk_x = static_cast<int32_t>(math::floor(pos.x / chunk_size));
        int32_t new_chunk_z = static_cast<int32_t>(math::floor(pos.z / chunk_size));

        /**
         * STEP 4: Check if chunk changed
         */
        if (new_chunk_x != chunk.chunk_x || new_chunk_z != chunk.chunk_y) {
            chunk.chunk_x = new_chunk_x;
            chunk.chunk_y = new_chunk_z;

            /**
             * STEP 5: Mark player as having changed chunks
             */
            registry.emplace_or_replace<PlayerChunkChangedTag>(entity);

            /**
             * STEP 6: Notify terrain module via Hub (HUB Pattern - WRITES)
             * Uses entity id as owner to notify terrain streaming system
             */
            uint32_t owner = static_cast<uint32_t>(entity);
            hub::set_or_create_hub_value(registry, owner, "PLR_CHK_X"_hs, static_cast<float>(new_chunk_x));
            hub::set_or_create_hub_value(registry, owner, "PLR_CHK_Z"_hs, static_cast<float>(new_chunk_z));
            hub::set_or_create_hub_value(registry, owner, "PLR_CHK_CHG"_hs, 1.0f);
        }
    }
}

void PlayerSpatialChunkSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerSpatialChunkSystem] Stopped");
}

}  // namespace ase::player
