/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_log_obsv_sys.cpp
 * @brief       PlayerLogObsvSystem - Log player state counts periodically
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    error/logging/output
 * @schedule    Conclusion
 * @created     2026-01-22
 * @modified    2026-04-13
 * @version     1.3.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_LOG_OBSV: Player State Observation Logging)
 *
 *   [Player Components + Tags]
 *          │
 *          │ state data (id, status, tags)
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerLogObsvSystem           │
 *   │                                             │
 *   │  READS:                                     │
 *   │    → PlayerMgrTag (manager entity)          │
 *   │    → PlayerStIdComponent (count players)    │
 *   │    → PlayerStStsComponent (state counts)    │
 *   │    → PlayerSpawnedTag (spawned count)       │
 *   │    → PlayerDirtyTag (dirty count)           │
 *   │    → PlayerChunkChangedTag (chunk changes)  │
 *   │    → "LOG_CONST_DEFAULT_INTERVAL"_hs (Hub)  │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    → PlayerCacheObsComponent (timer/cache)  │
 *   │    → log::debug (periodic output)           │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ debug log output
 *          ▼
 *   Console/Log File
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from hub_constants.json via Hub):
 *   "LOG_CONST_DEFAULT_INTERVAL"_hs → Log interval in seconds (float)
 *
 * WRITES (to Hub for other modules):
 *   (none - this is an observation/logging system)
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
 * [ ] Safe deletion (first collect, then delete)?
 * [ ] Not deleting other entities during iteration?
 * [ ] Not invalidating references during iteration?
 */

// INCLUDES - ONLY THESE ARE ALLOWED!
// FORBIDDEN: <vector>, <map>, <unordered_map>, <optional>, <algorithm>
// ALLOWED:   <cstdint>, <cmath>, <cassert>, ase-* headers

// Own header FIRST
#include <ase/player/systems/log/player_log_obsv_sys.hpp>
// Components from same module
#include <ase/player/components/cache/player_cache_obs_component.hpp>
#include <ase/player/components/tag/player_tag_mgr_component.hpp>
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/components/tag/player_tag_chunk_changed_component.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Hub for HUB Pattern
#include <ase/hub/api.hpp>
// Types for sentinel functions (is_not_found)
#include <ase/types/types.hpp>
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

/**
 * @brief Map player state index to human-readable name
 * @param idx State index (PLAYER_STATE_* from types.hpp)
 * @return Human-readable state name
 */
const char* state_name(uint8_t idx) {
    if (idx == PLAYER_STATE_IDLE) return "idle";
    if (idx == PLAYER_STATE_WALKING) return "walk";
    if (idx == PLAYER_STATE_RUNNING) return "run";
    if (idx == PLAYER_STATE_JUMPING) return "jump";
    if (idx == PLAYER_STATE_FALLING) return "fall";
    if (idx == PLAYER_STATE_SWIMMING) return "swim";
    if (idx == PLAYER_STATE_DEAD) return "dead";
    return "???";
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED — NO EXCEPTIONS!

void PlayerLogObsvSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerLogObsvSystem] Started");
}

void PlayerLogObsvSystem::tick(ecs::Registry& registry, float dt) {
    /**
     * STEP 1: Find player manager entity
     * PlayerMgrTag identifies the singleton manager entity.
     */
    auto mgr_view = registry.view<PlayerMgrTag>();

    for (auto mgr : mgr_view) {
        auto& cache = registry.get_or_emplace<PlayerCacheObsComponent>(mgr);

        /**
         * STEP 2: Update timer
         */
        cache.log_interval_timer += dt;

        /**
         * STEP 3: Count total players
         */
        uint32_t player_count = 0;
        auto player_view = registry.view<PlayerStIdComponent>();
        for (auto e : player_view) {
            (void)e;
            ++player_count;
        }

        /**
         * STEP 4: Count states using individual counters (NO arrays!)
         * Each state counted via named constant from types.hpp.
         */
        uint32_t idle_count = 0;
        uint32_t walking_count = 0;
        uint32_t running_count = 0;
        uint32_t jumping_count = 0;
        uint32_t falling_count = 0;

        auto status_view = registry.view<PlayerStStsComponent>();
        for (auto [e, status] : status_view.each()) {
            (void)e;
            if (status.sts == PLAYER_STATE_IDLE) { ++idle_count; continue; }
            if (status.sts == PLAYER_STATE_WALKING) { ++walking_count; continue; }
            if (status.sts == PLAYER_STATE_RUNNING) { ++running_count; continue; }
            if (status.sts == PLAYER_STATE_JUMPING) { ++jumping_count; continue; }
            if (status.sts == PLAYER_STATE_FALLING) { ++falling_count; continue; }
        }

        uint32_t moving_count = player_count - idle_count;

        /**
         * STEP 5: Detect significant change
         */
        bool significant_change =
            player_count != cache.last_player_count ||
            moving_count != cache.last_moving_count;

        /**
         * STEP 6: Read log interval from Hub (HUB Pattern - READS)
         * INLINED: No get_*() helper with Registry allowed.
         * Tick-system: silent fallback on NOT_FOUND. Missing constants are an
         * expected state when no replica/almanach is online — logging ERR every
         * tick would spam the log. Fallback to PLR_LOG_INTERVAL_FALLBACK.
         */
        float log_interval = hub::get(registry, hub::GLOBAL, "LOG_CONST_DEFAULT_INTERVAL"_hs);
        if (types::is_not_found(log_interval)) {
            log_interval = PLR_LOG_INTERVAL_FALLBACK;
        }

        /**
         * STEP 7: Log on interval or significant change
         */
        if (cache.log_interval_timer >= log_interval || significant_change) {
            cache.log_interval_timer = 0.0f;
            cache.last_player_count = player_count;
            cache.last_moving_count = moving_count;

            /**
             * STEP 8: Count tag-based states
             */
            uint32_t spawned_count = 0;
            auto spawned_view = registry.view<PlayerSpawnedTag>();
            for (auto e : spawned_view) {
                (void)e;
                ++spawned_count;
            }

            uint32_t dirty_count = 0;
            auto dirty_view = registry.view<PlayerDirtyTag>();
            for (auto e : dirty_view) {
                (void)e;
                ++dirty_count;
            }

            uint32_t chunk_changed_count = 0;
            auto chunk_view = registry.view<PlayerChunkChangedTag>();
            for (auto e : chunk_view) {
                (void)e;
                ++chunk_changed_count;
            }

            /**
             * STEP 9: Output debug log
             */
            log::debug("\x1b[38;5;141m[ase-player]\x1b[0m [PlayerLogObsvSystem] "
                       "players:{} -> idle:{} -> walk:{} -> run:{} -> jump:{} -> fall:{} -> "
                       "dirty:{} -> spawned:{} -> chk_chg:{}",
                       player_count, idle_count, walking_count, running_count,
                       jumping_count, falling_count, dirty_count, spawned_count,
                       chunk_changed_count);
        }
    }
}

void PlayerLogObsvSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerLogObsvSystem] Stopped");
}

}  // namespace ase::player
