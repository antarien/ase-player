/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_sta_chnk_sys.cpp
 * @brief       PlayerStaChnkSystem - Track player chunk position for streaming
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_STA_CHNK: Player Chunk Tracking)
 *
 *   [PlayerStaPosComponent from PlayerSimPhysSystem]
 *          │
 *          │ position updated
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerStaChnkSystem           │
 *   │                                             │
 *   │  READS:                                     │
 *   │    → PlayerStaPosComponent (x, z)            │
 *   │    → PlayerStaChkComponent (current chunk)   │
 *   │    → PlayerStMovComponent (chunk_size)      │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    → PlayerStaChkComponent (chunk_x, chunk_y)│
 *   │    → PlayerChnkChgdTag (if changed)     │
 *   │    → "PLR_CHK_CHG"_hs (Hub - notify terrain)│
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ chunk changed notification
 *          ▼
 *   TerrainStreamingSystem (via Hub)
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Components):
 *   PlayerStaPosComponent → Current position
 *   PlayerStaChkComponent → Current chunk
 *
 * WRITES (to Hub for other modules):
 *   "PLR_CHK_X"_hs   → Player chunk X coordinate (float)
 *   "PLR_CHK_Z"_hs   → Player chunk Z coordinate (float)
 *   "PLR_CHK_CHG"_hs → Player chunk changed notification (1.0f)
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
#include <ase/player/systems/state/player_sta_chnk_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_chk_comp.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>
#include <ase/player/components/tag/player_chnk_chgd_tag.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Hub for HUB Pattern
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>
// Math
#include <ase/math/math.hpp>

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

// No helper functions needed - all logic inlined in tick()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerStaChnkSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerStaChnkSystem] Started");
}

void PlayerStaChnkSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1 ENTFALLEN (S2b 2026-08-11): die Wabenadresse steht exakt in der Position -
     * dieses System braucht die Kantenlaenge nicht mehr, es liest die Adresse direkt ab.
     */

    /**
     * STEP 2: Process each player entity
     */
    auto view = registry.view<PlayerStaPosComponent, PlayerStaChkComponent>();

    for (auto [entity, pos, chunk] : view.each()) {
        /**
         * STEP 3: Die Wabenadresse steht seit S2b (2026-08-11) EXAKT in der Position -
         * keine floor-Ableitung aus Weltmetern mehr, kein Praezisionsverlust.
         */
        int32_t new_chunk_x = pos.chunk_x;
        int32_t new_chunk_z = pos.chunk_z;

        /**
         * STEP 4: Check if chunk changed
         */
        if (new_chunk_x != chunk.chunk_x || new_chunk_z != chunk.chunk_y) {
            chunk.chunk_x = new_chunk_x;
            chunk.chunk_y = new_chunk_z;

            /**
             * STEP 5: Mark player as having changed chunks
             */
            registry.emplace_or_replace<PlayerChnkChgdTag>(entity);

            /**
             * STEP 6: Notify terrain module via Hub (HUB Pattern - WRITES)
             * Uses entity id as owner to notify terrain streaming system
             */
            uint32_t owner = static_cast<uint32_t>(entity);
            hub::set(registry, owner, "PLR_CHK_X"_hs, static_cast<float>(new_chunk_x));
            hub::set(registry, owner, "PLR_CHK_Z"_hs, static_cast<float>(new_chunk_z));
            hub::set(registry, owner, "PLR_CHK_CHG"_hs, 1.0f);
        }
    }
}

void PlayerStaChnkSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerStaChnkSystem] Stopped");
}

}  // namespace ase::player
