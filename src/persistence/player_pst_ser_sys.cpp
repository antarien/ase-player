/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_pst_ser_sys.cpp
 * @brief       PlayerPstSerSystem - Player persistence serialization
 * @description Serializes dirty players and delegates persistence to Replication Layer.
 *              Naming: Player + Pst(Persist) + Ser(Serialize) + System
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    entity/actor/player
 * @schedule    Preservation
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     2.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_PST_SER: Player Persistence Serialization)
 *
 *   [PlayerPstDtyTag + PlayerSpawnedTag]
 *          │
 *          │ player dirty, needs persistence
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerPstSerSystem            │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - PlayerPstDtyTag (dirty flag)           │
 *   │    - PlayerSpawnedTag (spawned flag)        │
 *   │    - PlayerStIdComponent (identity)         │
 *   │    - PlayerStPosComponent (position)        │
 *   │    - PlayerStVelComponent (velocity)        │
 *   │    - PlayerStStsComponent (status)          │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - PlayerBufPstComponent (persistence buf)│
 *   │    - SerialBufJsnComponent (JSON buffer)    │
 *   │    - SerialJsnPndTag (serialization pnd)    │
 *   │    - Hub: REP_PST_SER, REP_PST_SYN          │
 *   │    - Removes PlayerPstDtyTag                │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ persistence request queued
 *          ▼
 *   SerialJsnSystem (serializes data)
 *
 * HUB Pattern (MIG_ASE_HUB_API v2.0):
 *
 * READS (from Components):
 *   PlayerStIdComponent → Player identity
 *   PlayerStPosComponent → Position data
 *
 * WRITES (to Hub for ase-replication):
 *   "REP_PST_SER"_hs → Serialization entity reference
 *   "REP_PST_SYN"_hs → Sync request flag
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
#include <ase/player/systems/persistence/player_pst_ser_sys.hpp>
// Components from same module
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/buffer/player_buf_pst_component.hpp>
#include <ase/player/components/tag/player_tag_pst_dty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/types.hpp>
// Serialization (Layer 2)
#include <ase/serial/serial.hpp>
// Hub for HUB Pattern
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>

namespace ase::player {
using namespace entt::literals;

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * IMPORTANT: Use anonymous namespace, NOT static keyword!
 *   OK: namespace { void helper() {...} }   // CORRECT
 *   NO: static void helper() {...}          // WRONG!
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerPstSerSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerPstSerSystem] Started");
}

void PlayerPstSerSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Process dirty players that need persistence
     * Single-pass processing without collect-then-process
     */
    auto view = registry.view<
        PlayerPstDtyTag,
        PlayerSpawnedTag,
        PlayerStIdComponent,
        PlayerStPosComponent,
        PlayerStVelComponent,
        PlayerStStsComponent
    >();

    /**
     * Use iterator pattern for entity creation during iteration
     */
    auto it = view.begin();
    auto end_it = view.end();
    while (it != end_it) {
        auto entity = *it;
        ++it;

        auto& id = registry.get<PlayerStIdComponent>(entity);
        auto& pos = registry.get<PlayerStPosComponent>(entity);

        /**
         * STEP 2: Create serialization request entity
         */
        auto ser = registry.create();

        auto& jsn_buf = registry.emplace<serial::SerialBufJsnComponent>(ser);
        jsn_buf.src_ptr = reinterpret_cast<uint64_t>(&pos);
        jsn_buf.src_typ = SERIAL_TYP_PLR_STA;
        jsn_buf.src_siz = sizeof(PlayerStPosComponent);
        jsn_buf.st = serial::SERIAL_ST_PND;

        registry.emplace<serial::SerialJsnPndTag>(ser);

        /**
         * Hub: Write persistence metadata (HUB Pattern - WRITES)
         * Replication module reads these to perform persistence
         */
        uint32_t owner = static_cast<uint32_t>(entity);
        hub::set(registry, owner, "REP_PST_SER"_hs, static_cast<float>(ser));
        hub::set(registry, owner, "REP_PST_SYN"_hs, 1.0f);

        /**
         * STEP 4: Update persistence buffer with player_id pointer
         * Uses pointer pattern, stores reference to id.player_id
         */
        auto& buf = registry.get_or_emplace<PlayerBufPstComponent>(entity);
        buf.plr_id_ptr = reinterpret_cast<uint64_t>(id.player_id);
        buf.plr_id_len = 0;
        while (buf.plr_id_len < sizeof(id.player_id) && id.player_id[buf.plr_id_len] != '\0') {
            ++buf.plr_id_len;
        }

        /**
         * STEP 5: Remove dirty tag (processed)
         */
        registry.remove<PlayerPstDtyTag>(entity);

        log::debug("[PlayerPstSerSystem] Queued player for persistence");
    }
}

void PlayerPstSerSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerPstSerSystem] Stopped");
}

}  // namespace ase::player
