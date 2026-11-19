/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_spwn_sys.cpp
 * @brief       PlayerHubSpwnSystem - turns the spawn rows of an embedding host into spawn requests
 * @description Takes every row HUB_PLR_SPWN_REQ, files a PlayerReqSpwnComponent request for it and
 *              releases the row; destroys its own answered requests in the next pass.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101557 - receive a spawn request
 * @actor       Player Presence Steward (PLAYPRESTE)
 * @category    hub
 * @schedule    Reception
 * @created     2026-11-13
 * @modified    2026-11-13
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (CAUSA_PLR_HUB_SPWN: the entry of a figure without a network frame)
 *
 *   [ase-pl-avatar AvatarHostSpwnSystem - one row HUB_PLR_SPWN_REQ per spawn request of the host]
 *          │
 *          │ the host lets its player enter the world
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubSpwnSystem           │
 *   │                                             │
 *   │  READS:                                     │
 *   │    → rows HUB_PLR_SPWN_REQ / HUB_PLR_SPWN   │
 *   │    → PlayerReqSpwnResComponent +            │
 *   │      PlayerSpwnHubTag (answered requests)   │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    → PlayerReqSpwnComponent +               │
 *   │      PlayerSpwnHubTag (a request entity)    │
 *   │    → releases every row it read             │
 *   │    → destroys its answered requests         │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ the same request the network reception files
 *          ▼
 *   PlayerLifeSpwnSystem (Dynamics) creates the figure, PlayerHubSpndSystem files HUB_PLR_SPND
 *
 * ONE REQUEST SHAPE FOR TWO ENTRIES. A server tier receives the spawn as a network frame
 * (PlayerSpwnRcvSystem); a device that embeds the engine has no frame, its host asks through a port
 * of ase-pl-avatar, and the plugin files the row this system reads. Both end in the same
 * PlayerReqSpwnComponent, so the figure is created, checked for a double and marked by one system.
 *
 * THE SEAT IS THE IDENTITY. The host names its player by a seat number; the player_id is
 * PLR_HUB_SPWN_SEAT_PREFIX followed by the seat in decimal. PlayerLifeSpwnSystem refuses an identity
 * that already stands, so a host that asks twice gets one figure.
 *
 * WHO FILES THE REQUEST REMOVES IT. PlayerLifeSpwnSystem answers a request with
 * PlayerReqSpwnResComponent and leaves the entity standing for the reader of the answer. The answer of
 * a host entry has no further reader, so this system destroys the requests it marked once they carry
 * their answer, in its next pass and before it files new ones.
 *
 * HUB Pattern (ARCH_ASE_HUB_API, Typisierte Zeilen):
 *
 * READS (from Hub):
 *   row HUB_PLR_SPWN_REQ → HUB_PLR_SPWN_SEAT (u32), HUB_PLR_SPWN_X (f32), HUB_PLR_SPWN_Z (f32),
 *                          read with hub::try_get_u32 / hub::try_get_f32
 *
 * WRITES (to Hub):
 *   hub::row_release of every row read, whatever its outcome
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
// FORBIDDEN: <vector>, <map>, <unordered_map>, <optional>, <algorithm>
// ALLOWED:   <cstdint>, <cmath>, <cassert>, ase-* headers

// Own header FIRST
#include <ase/player/systems/hub/player_hub_spwn_sys.hpp>
// Components from same module
#include <ase/player/components/request/player_req_chr_comp.hpp>
#include <ase/player/components/request/player_req_slot_comp.hpp>
#include <ase/player/components/request/player_req_spwn_comp.hpp>
#include <ase/player/components/request/player_req_spwn_res_comp.hpp>
#include <ase/player/components/tag/player_spwn_hub_tag.hpp>
#include <ase/player/types.hpp>
// Hub for the row API
#include <ase/hub/api.hpp>
// Sentinels and text
#include <ase/types/types.hpp>
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

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

// No helper functions needed - one request per row

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerHubSpwnSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubSpwnSystem] Started");
}

void PlayerHubSpwnSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    /**
     * STEP 1: The requests of the previous pass that carry their answer go - this system filed them,
     * and their answer has no reader beyond PlayerLifeSpwnSystem. Iterator pattern: the iterator steps
     * on before the entity it stands on is destroyed.
     */
    auto answered = registry.view<PlayerReqSpwnResComponent, PlayerSpwnHubTag>();
    auto answered_it = answered.begin();
    const auto answered_end = answered.end();
    while (answered_it != answered_end) {
        const ecs::Entity request = *answered_it;
        ++answered_it;
        const auto& result = answered.get<PlayerReqSpwnResComponent>(request);
        log::debug("[PlayerHubSpwnSystem] host entry answered: figure {} (created {})",
                   static_cast<uint32_t>(result.spawned_entity), result.success);
        registry.destroy(request);
    }

    /**
     * STEP 2: Every spawn row of the host - read checked, filed as a request, released whatever its
     * outcome
     */
    auto take = [&registry](ecs::Entity row) {
        const uint32_t row_id = static_cast<uint32_t>(row);
        uint32_t seat = ase::types::InvalidEntityId;
        float x = 0.0f;
        float z = 0.0f;
        if (!hub::try_get_u32(registry, row_id, "HUB_PLR_SPWN_SEAT"_hs, seat) ||
            !hub::try_get_f32(registry, row_id, "HUB_PLR_SPWN_X"_hs, x) ||
            !hub::try_get_f32(registry, row_id, "HUB_PLR_SPWN_Z"_hs, z)) {
            log::warn(log::WRN::CAT::INPUT_TRUNCATED, "PlayerHubSpwnSystem", row_id, "HUB_PLR_SPWN_REQ",
                      "a spawn row without its seat or its place lets no figure enter");
            hub::row_release(registry, row);
            return;
        }
        if (!ase::types::is_val_float(x) || !ase::types::is_val_float(z)) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerHubSpwnSystem", row_id, "HUB_PLR_SPWN_X", x);
            hub::row_release(registry, row);
            return;
        }

        const ecs::Entity request_entity = registry.create();
        auto& request = registry.emplace<PlayerReqSpwnComponent>(request_entity);
        const auto id_size = static_cast<uint32_t>(sizeof(request.player_id));
        utils::str_copy(request.player_id, id_size, PLR_HUB_SPWN_SEAT_PREFIX);
        utils::str_append_u64(request.player_id, id_size, static_cast<uint64_t>(seat));
        request.x = x;
        request.z = z;
        registry.emplace<PlayerSpwnHubTag>(request_entity);

        // The seat rides beside the request as a number: PlayerHubSlotPubSystem puts it on the figure the
        // answer names and files it as PLR_SEAT, the identity of the figure's last state (D2467).
        registry.emplace<PlayerReqSlotComponent>(request_entity).seat = seat;

        // The character the host named for the figure. A row without the field names none, and the
        // request then carries no character row; PlayerHubChrPubSystem reads only requests with one.
        uint32_t character_hash = 0u;
        if (hub::try_get_u32(registry, row_id, "HUB_PLR_SPWN_CHAR"_hs, character_hash)) {
            registry.emplace<PlayerReqChrComponent>(request_entity).character_hash = character_hash;
        }

        log::info("[PlayerHubSpwnSystem] host spawn request filed (player_id={} x={} z={})",
                  request.player_id, request.x, request.z);
        hub::row_release(registry, row);
    };
    hub::row_each(registry, "HUB_PLR_SPWN_REQ"_hs, "HUB_PLR_SPWN"_hs, take);
}

void PlayerHubSpwnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubSpwnSystem] Stopped");
}

}  // namespace ase::player
