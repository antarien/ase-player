/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_net_bct_snd_system.cpp
 * @brief       PlayerNetBctSndSystem - Process completed broadcasts and queue for cleanup
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    network
 * @schedule    Transmission
 * @created     2026-01-22
 * @modified    2026-01-22
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_NET_BCT_SND: Player Network Broadcast Send)
 *
 *   [SerialJsnFinTag + PlayerBctSpnSntTag/PlayerBctStaSntTag]
 *          │
 *          │ broadcast sent via Hub
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerNetBctSndSystem         │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - SerialJsnFinTag (serialization done)   │
 *   │    - SerialBufJsnComponent (JSON buffer)    │
 *   │    - PlayerBctSpnSntTag (spawn sent)        │
 *   │    - PlayerBctStaSntTag (state sent)        │
 *   │    - SerialErrTag (error occurred)          │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - PlayerDespPndTag (mark for cleanup)    │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ entities marked for cleanup
 *          ▼
 *   PlayerCleanupSystem (Schedule::Last)
 *
 * HUB Pattern (MIG_ASE_HUB)
 *
 * READS (from Components):
 *   SerialBufJsnComponent → Serialized JSON data
 *
 * WRITES (to Hub):
 *   (none - marks entities for deferred cleanup)
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
#include <ase/player/systems/network/player_net_bct_snd_system.hpp>
// Components from same module
#include <ase/player/components/buffer/player_buf_bct_spn_component.hpp>
#include <ase/player/components/buffer/player_buf_bct_sta_component.hpp>
#include <ase/player/components/tag/player_tag_bct_spn_pnd_component.hpp>
#include <ase/player/components/tag/player_tag_bct_sta_pnd_component.hpp>
#include <ase/player/components/tag/player_tag_bct_spn_snt_component.hpp>
#include <ase/player/components/tag/player_tag_bct_sta_snt_component.hpp>
#include <ase/player/components/tag/player_tag_desp_pnd_component.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Serialization (Layer 2)
#include <ase/serial/serial.hpp>
// Logging
#include <ase/log/log.hpp>

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

void PlayerNetBctSndSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerNetBctSndSystem] Started");
}

void PlayerNetBctSndSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Process completed spawn broadcasts
     * Mark sent entities for cleanup (deferred deletion pattern)
     */
    {
        auto view = registry.view<
            serial::SerialJsnFinTag,
            serial::SerialBufJsnComponent,
            PlayerBctSpnSntTag
        >();

        for (auto [entity, jsn_buf] : view.each()) {
            if (jsn_buf.jsn_ptr != 0 && jsn_buf.jsn_len > 0) {
                log::debug("[PlayerNetBctSndSystem] Spawn broadcast completed ({} bytes)", jsn_buf.jsn_len);
            }

            /**
             * Mark for deferred cleanup (Schedule::Last cleanup system)
             */
            registry.emplace_or_replace<PlayerDespPndTag>(entity);
        }
    }

    /**
     * STEP 2: Process completed state broadcasts
     */
    {
        auto view = registry.view<
            serial::SerialJsnFinTag,
            serial::SerialBufJsnComponent,
            PlayerBctStaSntTag
        >();

        for (auto [entity, jsn_buf] : view.each()) {
            /**
             * Mark for deferred cleanup
             */
            registry.emplace_or_replace<PlayerDespPndTag>(entity);
        }
    }

    /**
     * STEP 3: Handle serialization errors
     */
    {
        auto view = registry.view<
            serial::SerialErrTag,
            serial::SerialBufJsnComponent
        >();

        for (auto [entity, jsn_buf] : view.each()) {
            if (jsn_buf.err_ptr != 0) {
                log::error("[PlayerNetBctSndSystem] Serialization error occurred");
            }

            /**
             * Mark for deferred cleanup
             */
            registry.emplace_or_replace<PlayerDespPndTag>(entity);
        }
    }
}

void PlayerNetBctSndSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerNetBctSndSystem] Stopped");
}

}  // namespace ase::player
