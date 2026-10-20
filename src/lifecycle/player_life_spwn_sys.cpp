/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_life_spwn_sys.cpp
 * @brief       PlayerLifeSpwnSystem - Process player spawn requests and create the player row
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:5130 - play a character
 * @actor       SS/PC
 * @category    ecs/entity/entitylifecycle
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-10-17
 * @version     2.1.0
 *
 * WHAT THIS FILE CARRIES AND WHAT IT DOES NOT
 *
 * Birth and death are two lifecycles, not two paragraphs of one - they read different request
 * components, write different results and share nothing but the identity index. The errand of a
 * walker is a concern of its own, with its own request component, state component, two tags,
 * warning and log line.
 *
 *   the errand    → PlayerLifeRoamIniSystem (player_life_roam_ini_sys.cpp)
 *   despawn       → PlayerLifeDespSystem    (player_life_desp_sys.cpp)
 *
 * This file answers PlayerReqSpwnComponent and creates the player row. Both other systems run
 * AFTER it in Dynamics.
 *
 * CAUSAL CHAIN (CAUSA_PLR_LIFE_SPWN: a player row is born)
 *
 *   [PlayerReqSpwnComponent, and on a request that asked for a walker its companion
 *    PlayerReqRoamComponent, which this system leaves untouched]
 *          │
 *          │ spawn requests from Integration Layer
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerLifeSpwnSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    → PlayerReqSpwnComponent (requests)      │
 *   │    → PlayerStaIdntComponent (existing check)│
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    → PlayerStaIdntComponent (create)        │
 *   │    → PlayerStaPosComponent (create)         │
 *   │    → PlayerStaYawComponent (create)         │
 *   │    → PlayerStaVelComponent (create)         │
 *   │    → PlayerStaPhysComponent (create)        │
 *   │    → PlayerStaStsComponent (create)         │
 *   │    → PlayerStaChkComponent (create)         │
 *   │    → PlayerSpndTag (create)                 │
 *   │    → hub row HUB_LIFE_ALIV_PLR (life ask)   │
 *   │    → PlayerDrtyTag (create)                 │
 *   │    → PlayerReqSpwnResComponent (result)     │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ player entities created with the spawn mark and the life ask
 *          ▼
 *   PlayerLifeRoamIniSystem reads the result and sets a walker on its errand;
 *   PlayerHubSpndSystem publishes PlayerSpndTag as the hub row HUB_PLR_SPND, which observer
 *   systems in other modules walk to add their components (input, camera, terrain streaming);
 *   LifecycleHubAlivPlyrRcvSystem (ase-lifecycle) adopts HUB_LIFE_ALIV in Ingestion of the next
 *   tick
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   (none - the ground height arrives one tick later through the SYN pair of this module)
 *
 * WRITES (to Hub for other modules):
 *   hub row HUB_LIFE_ALIV_PLR - hub::row_create with the field HUB_LIFE_ALIV_PLR_BODY, the ask
 *   for the life row; HUB_LIFE_ALIV itself has one writer, ase-lifecycle. The figure row
 *   HUB_PLR_SPND is written by PlayerHubSpndSystem
 *
 * NOTE: This system ONLY creates player components. Other modules observe the figure row and add
 * their own components (input, camera, terrain streaming).
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
#include <ase/player/systems/lifecycle/player_life_spwn_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/request/player_req_spwn_comp.hpp>
#include <ase/player/components/request/player_req_spwn_res_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_yaw_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_phys_comp.hpp>
#include <ase/player/components/state/player_sta_sts_comp.hpp>
#include <ase/player/components/state/player_sta_chk_comp.hpp>
#include <ase/player/components/tag/player_drty_tag.hpp>
#include <ase/player/components/tag/player_mgr_tag.hpp>
#include <ase/player/components/tag/player_spnd_tag.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Hub API - the life ask HUB_LIFE_ALIV_PLR; ase-lifecycle adopts the life row HUB_LIFE_ALIV, on
// which the observer adoption filters (TerrainStrmObsSyncSystem PASS 2). Both live in the hub so
// that no module includes another one for them.
#include <ase/hub/api.hpp>
// Containers SSOT - the player index in tick() (NO std:: maps)
#include <ase/containers/hash_map.hpp>
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

// No helper functions needed - all logic inlined in system methods. The draw sequence of a
// walker's first pause and first course lives entirely in PlayerLifeRoamIniSystem; it is ONE
// statement in types.hpp (plr_roam_fraction, plr_roam_span), shared with the two phase systems.

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
         * The manager entity carries no movement settings: they are constants of types.hpp, and
         * a component filled with constants would state them a second time on every entity
         * (CLAUDE.md: "'Config-Components' with constants = FORBIDDEN"). The manager tag itself
         * marks the singleton entity.
         */
        log::info("[PlayerLifeSpwnSystem] Created player manager entity");
    }
}

void PlayerLifeSpwnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Movement settings come straight from types.hpp - every field of them only ever
     * holds a MOVEMENT_DEFAULT_* constant.
     */

    /**
     * THE PLAYER INDEX IS BUILT ONCE PER PASS, NOT ONCE PER REQUEST.
     *
     * The question is "is there already a row with this player_id?". A full
     * view<PlayerStaIdntComponent> INSIDE the request loop would be M requests times N players;
     * the cost would look cheap, because the view would stand in one line.
     *
     * The despawn loop stands in PlayerLifeDespSystem and builds its own index there, once per
     * tick, O(N). That is no relapse into the defect of the loop: the defect lies in the LOOP,
     * not in the number of systems. A shared map would have to live in a component, and a
     * HashMap is no POD.
     *
     * The index is a PREFILTER, not an answer. It maps hash to entity; whoever has a hit compares
     * the identifier at the found place again. Without that re-check a hash collision would not
     * be a missing answer but a PLAUSIBLY WRONG one: the wrong player, silently found. With the
     * re-check the worst case is a "not found".
     */
    containers::HashMap<uint64_t, ecs::Entity> player_by_id;
    for (auto [pe, identity] : registry.view<PlayerStaIdntComponent>().each()) {
        uint64_t h = PLR_ID_FNV_OFFSET;
        for (uint32_t i = 0;
             i < static_cast<uint32_t>(sizeof(identity.player_id)) - 1u
             && identity.player_id[i] != '\0';
             ++i) {
            h = (h ^ static_cast<uint8_t>(identity.player_id[i])) * PLR_ID_FNV_PRIME;
        }
        player_by_id.emplace(h, pe);
    }

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

        // Check if the player already exists - O(1) against the index, identifier re-checked
        uint64_t req_hash = PLR_ID_FNV_OFFSET;
        for (uint32_t i = 0;
             i < static_cast<uint32_t>(sizeof(request.player_id)) - 1u
             && request.player_id[i] != '\0';
             ++i) {
            req_hash = (req_hash ^ static_cast<uint8_t>(request.player_id[i])) * PLR_ID_FNV_PRIME;
        }

        bool player_exists = false;
        auto spawn_hit = player_by_id.find(req_hash);
        if (spawn_hit != player_by_id.end()) {
            const auto* known = registry.try_get<PlayerStaIdntComponent>(spawn_hit->second);
            if (known != nullptr &&
                std::strncmp(known->player_id, request.player_id,
                             sizeof(known->player_id) - 1) == 0) {
                player_exists = true;
            }
        }

        if (!player_exists) {
            /* THE HEIGHT IS NOT FETCHED HERE, IT BELONGS TO THE SYN PAIR.
             *
             * A hub read next to the `math::floor` of the chunk axes further down would put hub
             * I/O and deterministic arithmetic into ONE system: HUB_IO_MIXED_WITH_MATH.
             *
             * A NEW SYNC SYSTEM WOULD BE THE WRONG ANSWER. `player_sync_inp_sys.cpp` already reads
             * the same key and lays it into a bridge component that, by its own comment, has
             * "exactly one reader".
             *
             * And this spawn CANNOT read the bridge: it hangs on a player that does not exist yet
             * in this line. So it is born at y=0, and the SYN pair pulls the height in the next
             * tick once the chunk is loaded. What falls away is a shortcut by ONE tick, not the
             * height.
             *
             * The sentinel lesson stands where the read lies: `is_val_float` instead of
             * `!is_not_found`, because the value becomes a COORDINATE. */
            float ground_y = 0.0f;

            // Create player entity with ONLY player components
            result_entity = registry.create();

            auto& identity = registry.emplace<PlayerStaIdntComponent>(result_entity);
            std::strncpy(identity.player_id, request.player_id, sizeof(identity.player_id) - 1);
            identity.player_id[sizeof(identity.player_id) - 1] = '\0';

            /**
             * THE INDEX IS CARRIED ALONG, not only read. Two requests with the same identifier
             * in the same pass would otherwise create two players. PlayerLifeDespSystem builds
             * its index AFTER this pass and sees the freshly created player anyway, but inside
             * THIS loop carrying it along is the only guard against the double creation.
             */
            player_by_id.emplace(req_hash, result_entity);

            // Timestamps initialized to 0 - can be set via Hub or request if needed
            identity.spawned_at_ms = 0;
            identity.last_input_ms = 0;

            /**
             * CHUNK-RELATIVE: the wire delivers world metres (f32), kept is the exact cell
             * address plus small local metres; at place 7 (204 million world metres, ULP 16 m)
             * every step would otherwise vanish (pattern character_life_spwn_sys.cpp).
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

            /* THE FIGURE STANDS IN THE WORLD, its own mark says so. PlayerHubSpndSystem carries
             * it as the row HUB_PLR_SPND into the star; observers in other modules (input,
             * camera, terrain streaming) walk that kind. The hub write stands there and not here,
             * because this system computes the chunk axes (WRFL_ASE_SYN_PATTERN). */
            registry.emplace<PlayerSpndTag>(result_entity);
            registry.emplace<PlayerDrtyTag>(result_entity);

            /**
             * THE LIFE ASK. A settler LIVES, and whoever lives is observed: the observer adoption
             * (TerrainStrmObsSyncSystem PASS 2) sees only the rows of the kind HUB_LIFE_ALIV.
             * Without it the settler would wander invisibly, cell crossings without a single cell
             * mark. Its soul is led by the SESSION (MS/PC): BdiAgtRegSystem leaves out every
             * HUB_PLR_SPND row, so that no second will arises here.
             *
             * HUB_LIFE_ALIV has one writer, ase-lifecycle. This system files the ask
             * HUB_LIFE_ALIV_PLR; LifecycleHubAlivPlyrRcvSystem adopts the figure in Ingestion of
             * the next tick, before Dynamics, so the observers see it one tick after the spawn.
             */
            const ecs::Entity life_ask = hub::row_create(registry, "HUB_LIFE_ALIV_PLR"_hs);
            hub::set_u32(registry, static_cast<uint32_t>(life_ask), "HUB_LIFE_ALIV_PLR_BODY"_hs,
                         static_cast<uint32_t>(result_entity));

            success = true;
            log::debug("[PlayerLifeSpwnSystem] Spawned player");
        } else {
            log::debug("[PlayerLifeSpwnSystem] Player already exists");
        }

        /**
         * THE RESULT IS THE SEAM TO THE ERRAND
         *
         * PlayerLifeRoamIniSystem reads exactly this pair, the result plus the
         * PlayerReqRoamComponent that travelled with the request, and sets the walker on its
         * way. This system does NOT touch the errand and does NOT remove it: it is consumed
         * there, one system later in the same tick.
         */
        auto& result = registry.emplace<PlayerReqSpwnResComponent>(request_entity);
        result.spawned_entity = result_entity;
        result.success = success;

        registry.remove<PlayerReqSpwnComponent>(request_entity);
    }
}

void PlayerLifeSpwnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerLifeSpwnSystem] Stopped");
}

}  // namespace ase::player
