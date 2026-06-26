/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_life_spwn_sys.cpp
 * @brief       PlayerLifeSpwnSystem - Process player spawn and despawn requests
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    ecs/entity/entitylifecycle
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_LIFE_SPWN: Player Lifecycle Management)
 *
 *   [PlayerReqSpawnComponent / PlayerReqDespComponent]
 *          │
 *          │ spawn/despawn requests from Integration Layer
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerLifeSpwnSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - PlayerReqSpawnComponent (requests)     │
 *   │    - PlayerReqDespComponent (requests)      │
 *   │    - PlayerStIdComponent (existing check)   │
 *   │    - "TRN_HGT_AT_POS"_hs (Hub - height)     │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - PlayerStIdComponent (create)           │
 *   │    - PlayerStPosComponent (create)          │
 *   │    - PlayerStVelComponent (create)          │
 *   │    - PlayerStPhysComponent (create)         │
 *   │    - PlayerStStsComponent (create)          │
 *   │    - PlayerStChkComponent (create)          │
 *   │    - PlayerSpawnedTag (create)              │
 *   │    - PlayerDirtyTag (create)                │
 *   │    - PlayerReqSpawnResComponent (result)    │
 *   │    - PlayerReqDespResComponent (result)     │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ player entities created with PlayerSpawnedTag
 *          ▼
 *   Observer systems in other modules add their components
 *   (InputSpawnObserver, CameraSpawnObserver, etc.)
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   "TRN_HGT_AT_POS"_hs → Terrain height at position (set by terrain module)
 *
 * WRITES (to Hub for other modules):
 *   (none - observer systems read PlayerSpawnedTag directly)
 *
 * NOTE: This system ONLY creates player components. Other modules observe
 * PlayerSpawnedTag and add their own components (input, camera, terrain streaming).
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
 * [ ] on_start/on_stop: log::info with system name
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
#include <ase/player/systems/lifecycle/player_life_spwn_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/request/player_req_spawn_component.hpp>
#include <ase/player/components/request/player_req_desp_component.hpp>
#include <ase/player/components/request/player_req_spawn_res_component.hpp>
#include <ase/player/components/request/player_req_desp_res_component.hpp>
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_phys_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/state/player_st_chk_component.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/components/tag/player_tag_mgr_component.hpp>
#include <ase/player/components/tag/player_tag_desp_pnd_component.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Hub for HUB Pattern (cross-module reads)
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>
// Math
#include <ase/math/math.hpp>

#include <cstring>

namespace ase::player {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * IMPORTANT: Use anonymous namespace, NOT static keyword!
 *   OK: namespace { void helper() {...} }   // CORRECT
 *   NO: static void helper() {...}          // WRONG!
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

// No helper functions needed - all logic inlined in system methods

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerLifeSpwnSystem::on_start(ecs::Registry& registry) {
    log::info("[PlayerLifeSpwnSystem] Started");

    auto view = registry.view<PlayerMgrTag>();
    bool mgr_exists = false;
    for (auto e : view) {
        (void)e;
        mgr_exists = true;
        break;
    }

    if (!mgr_exists) {
        auto mgr = registry.create();
        registry.emplace<PlayerMgrTag>(mgr);

        auto& mov = registry.emplace<PlayerStMovComponent>(mgr);
        mov.walk_speed = MOVEMENT_DEFAULT_WALK_SPEED;
        mov.run_speed = MOVEMENT_DEFAULT_RUN_SPEED;
        mov.jump_impulse = MOVEMENT_DEFAULT_JUMP_IMPULSE;
        mov.gravity = MOVEMENT_DEFAULT_GRAVITY;
        mov.ground_friction = MOVEMENT_DEFAULT_GROUND_FRICTION;
        mov.air_control = MOVEMENT_DEFAULT_AIR_CONTROL;
        mov.ground_snap_dist = MOVEMENT_DEFAULT_GROUND_SNAP_DIST;
        mov.turn_speed = MOVEMENT_DEFAULT_TURN_SPEED;
        mov.min_speed_threshold = MOVEMENT_DEFAULT_MIN_SPEED_THRESHOLD;
        mov.velocity_epsilon = MOVEMENT_DEFAULT_VELOCITY_EPSILON;
        mov.eye_height = MOVEMENT_DEFAULT_EYE_HEIGHT;
        mov.chunk_size = MOVEMENT_DEFAULT_CHUNK_SIZE;

        log::info("[PlayerLifeSpwnSystem] Created player manager entity");
    }
}

void PlayerLifeSpwnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Get movement settings from manager
     */
    PlayerStMovComponent mov;
    mov.walk_speed = MOVEMENT_DEFAULT_WALK_SPEED;
    mov.run_speed = MOVEMENT_DEFAULT_RUN_SPEED;
    mov.jump_impulse = MOVEMENT_DEFAULT_JUMP_IMPULSE;
    mov.gravity = MOVEMENT_DEFAULT_GRAVITY;
    mov.ground_friction = MOVEMENT_DEFAULT_GROUND_FRICTION;
    mov.air_control = MOVEMENT_DEFAULT_AIR_CONTROL;
    mov.ground_snap_dist = MOVEMENT_DEFAULT_GROUND_SNAP_DIST;
    mov.turn_speed = MOVEMENT_DEFAULT_TURN_SPEED;
    mov.min_speed_threshold = MOVEMENT_DEFAULT_MIN_SPEED_THRESHOLD;
    mov.velocity_epsilon = MOVEMENT_DEFAULT_VELOCITY_EPSILON;
    mov.eye_height = MOVEMENT_DEFAULT_EYE_HEIGHT;
    mov.chunk_size = MOVEMENT_DEFAULT_CHUNK_SIZE;

    auto mov_view = registry.view<PlayerStMovComponent>();
    for (auto [e, existing_mov] : mov_view.each()) {
        (void)e;
        mov = existing_mov;
        break;
    }

    /**
     * STEP 2: Process local spawn requests (iterator pattern for entity creation)
     */
    auto spawn_view = registry.view<PlayerReqSpawnComponent>();
    auto spawn_it = spawn_view.begin();
    auto spawn_end = spawn_view.end();
    while (spawn_it != spawn_end) {
        auto request_entity = *spawn_it;
        ++spawn_it;

        auto& request = registry.get<PlayerReqSpawnComponent>(request_entity);
        ecs::Entity result_entity = ecs::NullEntity;
        bool success = false;

        // Check if player already exists
        bool player_exists = false;
        auto player_view = registry.view<PlayerStIdComponent>();
        for (auto [pe, identity] : player_view.each()) {
            (void)pe;
            if (std::strncmp(identity.player_id, request.player_id,
                             sizeof(identity.player_id) - 1) == 0) {
                player_exists = true;
                break;
            }
        }

        if (!player_exists) {
            // Get terrain height via Hub (HUB Pattern - READS)
            float ground_y = 0.0f;
            uint32_t pos_hash = static_cast<uint32_t>(
                static_cast<int32_t>(request.x) * 73856093 ^
                static_cast<int32_t>(request.z) * 19349663);
            float hub_height = hub::get(registry, pos_hash, "TRN_HGT_AT_POS"_hs);
            if (hub_height != hub::NOT_FOUND) {
                ground_y = hub_height;
            }
            // Note: If terrain height not in Hub, spawn at y=0 (terrain module will update)

            // Create player entity with ONLY player components
            result_entity = registry.create();

            auto& identity = registry.emplace<PlayerStIdComponent>(result_entity);
            std::strncpy(identity.player_id, request.player_id, sizeof(identity.player_id) - 1);
            identity.player_id[sizeof(identity.player_id) - 1] = '\0';
            // Timestamps initialized to 0 - can be set via Hub or request if needed
            identity.spawned_at_ms = 0;
            identity.last_input_ms = 0;

            auto& pos = registry.emplace<PlayerStPosComponent>(result_entity);
            pos.x = request.x;
            pos.y = ground_y;
            pos.z = request.z;
            pos.yaw = 0.0f;

            registry.emplace<PlayerStVelComponent>(result_entity);

            auto& physics = registry.emplace<PlayerStPhysComponent>(result_entity);
            physics.on_ground = true;
            physics.gravity_enabled = true;

            auto& sts = registry.emplace<PlayerStStsComponent>(result_entity);
            sts.sts = PLAYER_STATE_IDLE;

            auto& chunk = registry.emplace<PlayerStChkComponent>(result_entity);
            chunk.chunk_x = static_cast<int32_t>(math::floor(request.x / mov.chunk_size));
            chunk.chunk_y = static_cast<int32_t>(math::floor(request.z / mov.chunk_size));

            // Lifecycle tags - observer systems in other modules will see these
            // and add their own components (input, camera, terrain streaming)
            registry.emplace<PlayerSpawnedTag>(result_entity);
            registry.emplace<PlayerDirtyTag>(result_entity);

            success = true;
            log::debug("[PlayerLifeSpwnSystem] Spawned player");
        } else {
            log::debug("[PlayerLifeSpwnSystem] Player already exists");
        }

        auto& result = registry.emplace<PlayerReqSpawnResComponent>(request_entity);
        result.spawned_entity = result_entity;
        result.success = success;

        registry.remove<PlayerReqSpawnComponent>(request_entity);
    }

    /**
     * STEP 3: Process local despawn requests (use deferred deletion)
     */
    auto desp_view = registry.view<PlayerReqDespComponent>();
    auto desp_it = desp_view.begin();
    auto desp_end = desp_view.end();
    while (desp_it != desp_end) {
        auto request_entity = *desp_it;
        ++desp_it;

        auto& request = registry.get<PlayerReqDespComponent>(request_entity);
        bool success = false;

        // Find and mark player for deletion
        auto player_view = registry.view<PlayerStIdComponent>();
        for (auto [pe, identity] : player_view.each()) {
            if (std::strncmp(identity.player_id, request.player_id,
                             sizeof(identity.player_id) - 1) == 0) {
                registry.emplace_or_replace<PlayerDespPndTag>(pe);
                success = true;
                log::debug("[PlayerLifeSpwnSystem] Marked player for despawn");
                break;
            }
        }

        if (!success) {
            log::debug("[PlayerLifeSpwnSystem] Player not found for despawn");
        }

        auto& result = registry.emplace<PlayerReqDespResComponent>(request_entity);
        result.success = success;

        registry.remove<PlayerReqDespComponent>(request_entity);
    }

    /**
     * STEP 4: Destroy entities tagged for despawn (deferred deletion)
     */
    auto pnd_view = registry.view<PlayerDespPndTag>();
    auto pnd_it = pnd_view.begin();
    auto pnd_end = pnd_view.end();
    while (pnd_it != pnd_end) {
        auto entity = *pnd_it;
        ++pnd_it;
        registry.destroy(entity);
    }
}

void PlayerLifeSpwnSystem::on_stop(ecs::Registry& registry) {
    log::info("[PlayerLifeSpwnSystem] Stopping");

    // Tag all players for despawn
    auto view = registry.view<PlayerStIdComponent>();
    uint32_t count = 0;
    for (auto entity : view) {
        registry.emplace_or_replace<PlayerDespPndTag>(entity);
        ++count;
    }

    // Destroy all tagged entities
    auto pnd_view = registry.view<PlayerDespPndTag>();
    auto pnd_it = pnd_view.begin();
    auto pnd_end = pnd_view.end();
    while (pnd_it != pnd_end) {
        auto entity = *pnd_it;
        ++pnd_it;
        registry.destroy(entity);
    }

    if (count > 0) {
        log::info("[PlayerLifeSpwnSystem] Despawned {} players on shutdown", count);
    }

    log::info("[PlayerLifeSpwnSystem] Stopped");
}

}  // namespace ase::player
