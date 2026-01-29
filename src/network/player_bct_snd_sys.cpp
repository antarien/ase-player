/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_bct_snd_sys.cpp
 * @brief       PlayerBctSndSystem - Player broadcast send system
 * @description Sends serialized player broadcasts via Hub for replication.
 *              Pure ECS pattern: reads SerialJsnFinTag, writes Hub values.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    entity/actor/player
 * @schedule    Transmission
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     2.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_BCT_SND: Player Broadcast Send)
 *
 *   [SerialJsnFinTag + PlayerBctSpnPndTag/PlayerBctStaPndTag]
 *          │
 *          │ serialization complete
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerBctSndSystem            │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - SerialJsnFinTag (serialization done)   │
 *   │    - SerialBufJsnComponent (JSON buffer)    │
 *   │    - PlayerBufBctSpnComponent (spawn data)  │
 *   │    - PlayerBufBctStaComponent (state data)  │
 *   │    - PlayerBctSpnPndTag (spawn pending)     │
 *   │    - PlayerBctStaPndTag (state pending)     │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - Hub: REP_MSG_CHN, REP_MSG_PTR_*        │
 *   │    - Hub: REP_MSG_LEN, REP_MSG_BCT          │
 *   │    - PlayerBctSpnSntTag (spawn sent)        │
 *   │    - PlayerBctStaSntTag (state sent)        │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ message data written to Hub
 *          ▼
 *   ReplicationSndSystem (reads Hub, sends via network)
 *
 * HUB Pattern (MIG_ASE_HUB_API v2.0):
 *
 * READS (from ase-serial via Components):
 *   SerialBufJsnComponent → Serialized JSON data
 *
 * WRITES (to Hub for ase-replication):
 *   "REP_MSG_CHN"_hs    → Channel name hash
 *   "REP_MSG_PTR_HI"_hs → Payload pointer high bits
 *   "REP_MSG_PTR_LO"_hs → Payload pointer low bits
 *   "REP_MSG_LEN"_hs    → Payload length
 *   "REP_MSG_BCT"_hs    → Broadcast request flag (1.0)
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
#include <ase/player/systems/network/player_bct_snd_sys.hpp>
// Components from same module
#include <ase/player/components/buffer/player_buf_bct_spn_component.hpp>
#include <ase/player/components/buffer/player_buf_bct_sta_component.hpp>
#include <ase/player/components/tag/player_tag_bct_spn_pnd_component.hpp>
#include <ase/player/components/tag/player_tag_bct_sta_pnd_component.hpp>
#include <ase/player/components/tag/player_tag_bct_spn_snt_component.hpp>
#include <ase/player/components/tag/player_tag_bct_sta_snt_component.hpp>
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

void PlayerBctSndSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerBctSndSystem] Started");
}

void PlayerBctSndSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Process spawn broadcasts (Tag-filtered View)
     * Entities with SerialJsnFinTag + PlayerBctSpnPndTag, excluding already sent
     */
    {
        auto view = registry.view<serial::SerialJsnFinTag,
                                  serial::SerialBufJsnComponent,
                                  PlayerBufBctSpnComponent,
                                  PlayerBctSpnPndTag>(
            entt::exclude<PlayerBctSpnSntTag>
        );

        for (auto [entity, jsn_buf, spn_buf] : view.each()) {
            if (jsn_buf.st != serial::SERIAL_ST_FIN || jsn_buf.jsn_ptr == 0) {
                continue;
            }

            uint32_t owner = static_cast<uint32_t>(entity);

            /**
             * Hub: Write broadcast data (HUB Pattern - WRITES)
             * Replication module will read these values
             */
            hub::set(registry, owner, "REP_MSG_CHN"_hs, static_cast<float>(CHANNEL_PLR_SPN_HASH));
            hub::set(registry, owner, "REP_MSG_PTR_HI"_hs, static_cast<float>(jsn_buf.jsn_ptr >> 32));
            hub::set(registry, owner, "REP_MSG_PTR_LO"_hs, static_cast<float>(jsn_buf.jsn_ptr & 0xFFFFFFFF));
            hub::set(registry, owner, "REP_MSG_LEN"_hs, static_cast<float>(jsn_buf.jsn_len));
            hub::set(registry, owner, "REP_MSG_BCT"_hs, 1.0f);

            /**
             * Mark as sent (deferred, tags only)
             */
            registry.emplace_or_replace<PlayerBctSpnSntTag>(entity);

            log::debug("[PlayerBctSndSystem] Spawn broadcast queued via Hub");
        }
    }

    /**
     * STEP 2: Process state broadcasts (separate Tag-filtered View)
     */
    {
        auto view = registry.view<serial::SerialJsnFinTag,
                                  serial::SerialBufJsnComponent,
                                  PlayerBufBctStaComponent,
                                  PlayerBctStaPndTag>(
            entt::exclude<PlayerBctStaSntTag>
        );

        for (auto [entity, jsn_buf, sta_buf] : view.each()) {
            if (jsn_buf.st != serial::SERIAL_ST_FIN || jsn_buf.jsn_ptr == 0) {
                continue;
            }

            uint32_t owner = static_cast<uint32_t>(entity);

            /**
             * Hub: Write broadcast data (HUB Pattern - WRITES)
             */
            hub::set(registry, owner, "REP_MSG_CHN"_hs, static_cast<float>(CHANNEL_PLR_STA_HASH));
            hub::set(registry, owner, "REP_MSG_PTR_HI"_hs, static_cast<float>(jsn_buf.jsn_ptr >> 32));
            hub::set(registry, owner, "REP_MSG_PTR_LO"_hs, static_cast<float>(jsn_buf.jsn_ptr & 0xFFFFFFFF));
            hub::set(registry, owner, "REP_MSG_LEN"_hs, static_cast<float>(jsn_buf.jsn_len));
            hub::set(registry, owner, "REP_MSG_BCT"_hs, 1.0f);

            /**
             * Mark as sent (deferred, tags only)
             */
            registry.emplace_or_replace<PlayerBctStaSntTag>(entity);
        }
    }
}

void PlayerBctSndSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerBctSndSystem] Stopped");
}

}  // namespace ase::player
