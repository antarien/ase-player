/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_bct_req_system.cpp
 * @brief       PlayerBctReqSystem - Create broadcast request entities for player state changes
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    network
 * @schedule    Transmission
 * @created     2026-01-22
 * @modified    2026-01-22
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_BCT_REQ: Player Broadcast Request Creation)
 *
 *   [PlayerDirtyTag + PlayerSpawnedTag]
 *          │
 *          │ dirty/spawned players need broadcast
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerBctReqSystem            │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - PlayerStIdComponent (identity)         │
 *   │    - PlayerStPosComponent (position)        │
 *   │    - PlayerStVelComponent (velocity)        │
 *   │    - PlayerStStsComponent (state)           │
 *   │    - PlayerSpawnedTag (new players)         │
 *   │    - PlayerDirtyTag (changed players)       │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - PlayerBufBctSpnComponent (spawn buf)   │
 *   │    - PlayerBufBctStaComponent (state buf)   │
 *   │    - PlayerBctSpnPndTag (spawn pending)     │
 *   │    - PlayerBctStaPndTag (state pending)     │
 *   │    - Removes PlayerSpawnedTag               │
 *   │    - Removes PlayerDirtyTag                 │
 *   │    - Removes PlayerChunkChangedTag          │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ serialization entities created
 *          ▼
 *   PlayerBctSndSystem (sends via network)
 *
 * HUB Pattern (MIG_ASE_HUB)
 *
 * READS (from player module via Components):
 *   PlayerStIdComponent  → Player identity for broadcast
 *   PlayerStPosComponent → Position data
 *   PlayerStVelComponent → Velocity data
 *   PlayerStStsComponent → State data
 *
 * WRITES (to Hub for other modules):
 *   (none - creates serialization entities directly)
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
#include <ase/player/systems/network/player_bct_req_system.hpp>
// Components from same module
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/buffer/player_buf_bct_spn_component.hpp>
#include <ase/player/components/buffer/player_buf_bct_sta_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/components/tag/player_tag_chunk_changed_component.hpp>
#include <ase/player/components/tag/player_tag_bct_spn_pnd_component.hpp>
#include <ase/player/components/tag/player_tag_bct_sta_pnd_component.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Serialization (Layer 2)
#include <ase/serial/serial.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstring>

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

void PlayerBctReqSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerBctReqSystem] Started");
}

void PlayerBctReqSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Process spawned players - mark for broadcast
     * Tag entities with PlayerBctSpnReqTag, then process in separate pass.
     */
    auto spawn_view = registry.view<PlayerStIdComponent, PlayerStPosComponent, PlayerSpawnedTag>();

    /**
     * STEP 2: Create spawn broadcast entities using iterator-based loop
     * Using begin/end iterators allows entity creation during iteration.
     */
    auto spawn_it = spawn_view.begin();
    auto spawn_end = spawn_view.end();
    while (spawn_it != spawn_end) {
        auto entity = *spawn_it;
        ++spawn_it;

        auto& id = registry.get<PlayerStIdComponent>(entity);
        auto& pos = registry.get<PlayerStPosComponent>(entity);

        auto ser = registry.create();

        auto& spn_buf = registry.emplace<PlayerBufBctSpnComponent>(ser);
        std::strncpy(spn_buf.player_id, id.player_id, sizeof(spn_buf.player_id) - 1);
        spn_buf.player_id[sizeof(spn_buf.player_id) - 1] = '\0';
        spn_buf.spawned_at_ms = id.spawned_at_ms;
        spn_buf.last_input_ms = id.last_input_ms;
        spn_buf.x = pos.x;
        spn_buf.y = pos.y;
        spn_buf.z = pos.z;
        spn_buf.yaw = pos.yaw;

        auto& jsn_buf = registry.emplace<serial::SerialBufJsnComponent>(ser);
        jsn_buf.src_ptr = reinterpret_cast<uint64_t>(&registry.get<PlayerBufBctSpnComponent>(ser));
        jsn_buf.src_typ = SERIAL_TYP_PLR_SPN;
        jsn_buf.src_siz = sizeof(PlayerBufBctSpnComponent);
        jsn_buf.st = serial::SERIAL_ST_PND;

        registry.emplace<serial::SerialJsnPndTag>(ser);
        registry.emplace<PlayerBctSpnPndTag>(ser);

        registry.remove<PlayerSpawnedTag>(entity);

        log::debug("[PlayerBctReqSystem] Created spawn request for player {}", id.player_id);
    }

    /**
     * STEP 3: Process dirty players - create state broadcast requests
     */
    auto dirty_view = registry.view<PlayerStIdComponent, PlayerStPosComponent,
                                    PlayerStVelComponent, PlayerStStsComponent, PlayerDirtyTag>();

    auto dirty_it = dirty_view.begin();
    auto dirty_end = dirty_view.end();
    while (dirty_it != dirty_end) {
        auto entity = *dirty_it;
        ++dirty_it;

        auto& id = registry.get<PlayerStIdComponent>(entity);
        auto& pos = registry.get<PlayerStPosComponent>(entity);
        auto& vel = registry.get<PlayerStVelComponent>(entity);
        auto& sts = registry.get<PlayerStStsComponent>(entity);

        auto ser = registry.create();

        auto& sta_buf = registry.emplace<PlayerBufBctStaComponent>(ser);
        std::strncpy(sta_buf.player_id, id.player_id, sizeof(sta_buf.player_id) - 1);
        sta_buf.player_id[sizeof(sta_buf.player_id) - 1] = '\0';
        sta_buf.spawned_at_ms = id.spawned_at_ms;
        sta_buf.last_input_ms = id.last_input_ms;
        sta_buf.x = pos.x;
        sta_buf.y = pos.y;
        sta_buf.z = pos.z;
        sta_buf.yaw = pos.yaw;
        sta_buf.vx = vel.vx;
        sta_buf.vy = vel.vy;
        sta_buf.vz = vel.vz;
        sta_buf.sts = sts.sts;

        auto& jsn_buf = registry.emplace<serial::SerialBufJsnComponent>(ser);
        jsn_buf.src_ptr = reinterpret_cast<uint64_t>(&registry.get<PlayerBufBctStaComponent>(ser));
        jsn_buf.src_typ = SERIAL_TYP_PLR_STA;
        jsn_buf.src_siz = sizeof(PlayerBufBctStaComponent);
        jsn_buf.st = serial::SERIAL_ST_PND;

        registry.emplace<serial::SerialJsnPndTag>(ser);
        registry.emplace<PlayerBctStaPndTag>(ser);

        registry.remove<PlayerDirtyTag>(entity);
    }

    /**
     * STEP 4: Clear chunk changed tags
     */
    auto chunk_view = registry.view<PlayerChunkChangedTag>();
    auto chunk_it = chunk_view.begin();
    auto chunk_end = chunk_view.end();
    while (chunk_it != chunk_end) {
        auto entity = *chunk_it;
        ++chunk_it;
        registry.remove<PlayerChunkChangedTag>(entity);
    }
}

void PlayerBctReqSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerBctReqSystem] Stopped");
}

}  // namespace ase::player
