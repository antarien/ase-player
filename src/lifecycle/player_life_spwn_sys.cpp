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
 * @modified    2026-08-09
 * @version     1.2.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_LIFE_SPWN: Player Lifecycle Management)
 *
 *   [PlayerReqSpwnComponent / PlayerReqDespComponent, and on a spawn request that asked for a
 *    walker its companion PlayerReqRoamComponent]
 *          │
 *          │ spawn/despawn requests from Integration Layer
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerLifeSpwnSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - PlayerReqSpwnComponent (requests)     │
 *   │    - PlayerReqRoamComponent (the errand)    │
 *   │    - PlayerReqDespComponent (requests)      │
 *   │    - PlayerStaIdntComponent (existing check)   │
 *   │    - "TRN_HGT_AT_POS"_hs (Hub - height)     │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - PlayerStaIdntComponent (create)           │
 *   │    - PlayerStaPosComponent (create)          │
 *   │    - PlayerStaVelComponent (create)          │
 *   │    - PlayerStaPhysComponent (create)         │
 *   │    - PlayerStaStsComponent (create)          │
 *   │    - PlayerStaChkComponent (create)          │
 *   │    - PlayerStaRoamComponent (walkers only)  │
 *   │    - PlayerRoamRestTag (walkers only)       │
 *   │    - PlayerRoamRunTag (fast walkers only)   │
 *   │    - PlayerSpndTag (create)              │
 *   │    - PlayerDrtyTag (create)                │
 *   │    - PlayerReqSpwnResComponent (result)    │
 *   │    - PlayerReqDespResComponent (result)     │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ player entities created with PlayerSpndTag
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
 *   (none - observer systems read PlayerSpndTag directly)
 *
 * NOTE: This system ONLY creates player components. Other modules observe
 * PlayerSpndTag and add their own components (input, camera, terrain streaming).
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
// FORBIDDEN: <vector>, <map>, <unordered_map>, <optional>, <algorithm>, <chrono>
// ALLOWED:   <cstdint>, <cmath>, <cassert>, ase-* headers

// Own header FIRST
#include <ase/player/systems/lifecycle/player_life_spwn_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/request/player_req_spwn_comp.hpp>
#include <ase/player/components/request/player_req_roam_comp.hpp>
#include <ase/player/components/request/player_req_desp_comp.hpp>
#include <ase/player/components/request/player_req_spwn_res_comp.hpp>
#include <ase/player/components/request/player_req_desp_res_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_yaw_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_phys_comp.hpp>
#include <ase/player/components/state/player_sta_sts_comp.hpp>
#include <ase/player/components/state/player_sta_chk_comp.hpp>
#include <ase/player/components/state/player_sta_roam_comp.hpp>
#include <ase/player/components/tag/player_drty_tag.hpp>
#include <ase/player/components/tag/player_spnd_tag.hpp>
#include <ase/player/components/tag/player_mgr_tag.hpp>
#include <ase/player/components/tag/player_desp_pnd_tag.hpp>
#include <ase/player/components/tag/player_roam_rest_tag.hpp>
#include <ase/player/components/tag/player_roam_run_tag.hpp>
// Cross-module POD header shared via the include path (HARD RULE: header-only POD tags may be
// shared; the loader writes them into the SAME World registry - no compiled cross-link):
//   lifecycle::LifecycleAlivTag - the living marker the observer adoptions filter on
//   (TerrainStrmObsSyncSystem PASS 2, Betreiber-Kanalentscheid 2026-08-10; Muster
//   character_life_spwn_sys.cpp)
#include <ase/lifecycle/components/tag/status/lifecycle_tag_aliv_component.hpp>
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

// No helper functions needed - all logic inlined in system methods. The draw sequence a walker's
// first pause and first course come from is ONE statement in types.hpp (plr_roam_fraction,
// plr_roam_span), shared with the two phase systems.

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerLifeSpwnSystem::on_start(ecs::Registry& registry) {
    log::debug("[PlayerLifeSpwnSystem] Started");

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

        /**
         * The manager entity used to carry PlayerStMovComponent, filled with twelve
         * MOVEMENT_DEFAULT_* constants (2026-08-15 removed). Every consumer read the
         * same constants back, so the component was a per-entity duplicate of
         * types.hpp - forbidden by CLAUDE.md ("'Config-Components' with constants =
         * FORBIDDEN"). The manager tag itself stays: it marks the singleton entity.
         */
        log::info("[PlayerLifeSpwnSystem] Created player manager entity");
    }
}

void PlayerLifeSpwnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Movement settings come straight from types.hpp (2026-08-15).
     * They used to be copied into PlayerStMovComponent and read back through a
     * manager lookup; every field only ever held a MOVEMENT_DEFAULT_* constant.
     */

    /**
     * STEP 2: Process local spawn requests (iterator pattern for entity creation)
     */
    auto spawn_view = registry.view<PlayerReqSpwnComponent>();
    auto spawn_it = spawn_view.begin();
    auto spawn_end = spawn_view.end();
    while (spawn_it != spawn_end) {
        auto request_entity = *spawn_it;
        ++spawn_it;

        auto& request = registry.get<PlayerReqSpwnComponent>(request_entity);
        ecs::Entity result_entity = ecs::NullEntity;
        bool success = false;

        // Check if player already exists
        bool player_exists = false;
        auto player_view = registry.view<PlayerStaIdntComponent>();
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

            auto& identity = registry.emplace<PlayerStaIdntComponent>(result_entity);
            std::strncpy(identity.player_id, request.player_id, sizeof(identity.player_id) - 1);
            identity.player_id[sizeof(identity.player_id) - 1] = '\0';
            // Timestamps initialized to 0 - can be set via Hub or request if needed
            identity.spawned_at_ms = 0;
            identity.last_input_ms = 0;

            /**
             * CHUNK-RELATIV (S2b 2026-08-11): der Draht liefert Weltmeter (f32), gefuehrt wird
             * die Wabenadresse exakt plus kleine lokale Meter - an Ort 7 (204 Mio Weltmeter,
             * ULP 16 m) verpuffte sonst jeder Schritt (Muster character_life_spwn_sys.cpp).
             */
            auto& pos = registry.emplace<PlayerStaPosComponent>(result_entity);
            pos.chunk_x = static_cast<int32_t>(math::floor(request.x / MOVEMENT_DEFAULT_CHUNK_SIZE));
            pos.chunk_z = static_cast<int32_t>(math::floor(request.z / MOVEMENT_DEFAULT_CHUNK_SIZE));
            pos.local_x = request.x - static_cast<float>(pos.chunk_x) * MOVEMENT_DEFAULT_CHUNK_SIZE;
            pos.local_z = request.z - static_cast<float>(pos.chunk_z) * MOVEMENT_DEFAULT_CHUNK_SIZE;
            pos.y = ground_y;

            registry.emplace<PlayerStaYawComponent>(result_entity);

            registry.emplace<PlayerStaVelComponent>(result_entity);

            auto& physics = registry.emplace<PlayerStaPhysComponent>(result_entity);
            physics.on_ground = true;
            physics.gravity_enabled = true;

            auto& sts = registry.emplace<PlayerStaStsComponent>(result_entity);
            sts.sts = PLAYER_STATE_IDLE;

            auto& chunk = registry.emplace<PlayerStaChkComponent>(result_entity);
            chunk.chunk_x = pos.chunk_x;
            chunk.chunk_y = pos.chunk_z;

            // Lifecycle tags - observer systems in other modules will see these
            // and add their own components (input, camera, terrain streaming)
            registry.emplace<PlayerSpndTag>(result_entity);
            registry.emplace<PlayerDrtyTag>(result_entity);

            /**
             * DIE LEBENSMARKE - ein Siedler LEBT, und wer lebt, wird beobachtet: die
             * Beobachter-Adoption (TerrainStrmObsSyncSystem PASS 2) sieht ausschliesslich
             * lifecycle::LifecycleAlivTag (Betreiber-Kanalentscheid 2026-08-10). Ohne die Marke
             * wanderte der Siedler unsichtbar - 381 Wabenuebertritte, null Zellmarkierungen
             * (gemessen 2026-08-11). Seine Seele fuehrt die SITZUNG (MS/PC): BdiAgtRegSystem
             * excluded player::PlayerSpndTag, damit hier kein zweiter Wille entsteht.
             */
            registry.emplace<lifecycle::LifecycleAlivTag>(result_entity);

            /**
             * THE ERRAND IS BORN WITH THE PLAYER, and this is the ONE place where a requested
             * speed meets the movement authority. Above the walking gear the errand rides the
             * running one; above the running gear there is no gear left, so the magnitude caps
             * and the request is answered with a warning rather than silently obeyed - a speed
             * beyond the authority is what the cheat lever next door exists for, and a walker
             * that quietly became one would falsify every measurement taken downstream.
             *
             * The walker starts in the RESTING phase: it arrives, stands, and PlayerSimRestSystem
             * decides its first course when that first pause runs out. Setting off mid-stride
             * would need an initial heading decided here as well, and the course of a leg already
             * has exactly one decision maker.
             */
            const auto* errand = registry.try_get<PlayerReqRoamComponent>(request_entity);
            if (errand != nullptr) {
                const uint32_t owner = static_cast<uint32_t>(result_entity);
                const bool runs = errand->speed > MOVEMENT_DEFAULT_WALK_SPEED;
                const float gear = runs ? MOVEMENT_DEFAULT_RUN_SPEED : MOVEMENT_DEFAULT_WALK_SPEED;
                if (errand->speed > MOVEMENT_DEFAULT_RUN_SPEED) {
                    log::warn("[PlayerLifeSpwnSystem] Errand asked for {} m/s, the movement "
                              "authority carries {} m/s - the walker is capped, not exempted",
                              errand->speed, MOVEMENT_DEFAULT_RUN_SPEED);
                }
                auto& roam = registry.emplace<PlayerStaRoamComponent>(result_entity);
                roam.speed = errand->speed;
                roam.forward = math::min(errand->speed / gear, PLR_ROAM_INPUT_ENGAGED);
                roam.heading =
                    plr_roam_span(plr_roam_fraction(owner, roam.leg_index, PLR_ROAM_MIX_TURN),
                                  0.0f, math::TWO_PI);
                roam.leg_sec =
                    plr_roam_span(plr_roam_fraction(owner, roam.leg_index, PLR_ROAM_MIX_REST),
                                  PLR_ROAM_REST_MIN_SEC, PLR_ROAM_REST_MAX_SEC);
                registry.emplace<PlayerRoamRestTag>(result_entity);
                if (runs) {
                    registry.emplace<PlayerRoamRunTag>(result_entity);
                }
                log::info("[PlayerLifeSpwnSystem] Walker set on an errand at {} m/s "
                          "(input {}, first pause {} s)",
                          roam.speed, roam.forward, roam.leg_sec);
            }

            success = true;
            log::debug("[PlayerLifeSpwnSystem] Spawned player");
        } else {
            log::debug("[PlayerLifeSpwnSystem] Player already exists");
        }

        auto& result = registry.emplace<PlayerReqSpwnResComponent>(request_entity);
        result.spawned_entity = result_entity;
        result.success = success;

        registry.remove<PlayerReqSpwnComponent>(request_entity);
        // The errand travelled WITH the request and is consumed with it: leaving it behind would
        // let a later pass read an errand whose player already exists.
        registry.remove<PlayerReqRoamComponent>(request_entity);
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
        auto player_view = registry.view<PlayerStaIdntComponent>();
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
    auto view = registry.view<PlayerStaIdntComponent>();
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

    log::debug("[PlayerLifeSpwnSystem] Stopped");
}

}  // namespace ase::player
