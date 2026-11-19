/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_slot_pub_sys.cpp
 * @brief       PlayerHubSlotPubSystem - puts the seat of its device player on a new figure
 * @description Lays PlayerStaSlotComponent on every figure a host spawn request brought into being
 *              and files the seat as the per-entity hub value PLR_SEAT.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101637 - seat a figure
 * @actor       Player Presence Steward (PLAYPRESTE)
 * @category    hub
 * @schedule    Dynamics
 * @parity      server_only
 * @created     2026-11-19
 * @modified    2026-11-19
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (D2467, the identity of a figure's last state across a restart)
 *
 *   [PlayerHubSpwnSystem laid PlayerReqSlotComponent on a host spawn request; PlayerLifeSpwnSystem
 *    answered the request with PlayerReqSpwnResComponent]
 *          │
 *          ▼
 *   ┌───────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubSlotPubSystem                          │
 *   │  READS:  PlayerReqSlotComponent, PlayerReqSpwnResComponent    │
 *   │  WRITES: PlayerStaSlotComponent (figure), PLR_SEAT (figure)   │
 *   └───────────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   [ase-hub files PLR_SEAT as the identity of the figure's last state (last_state identity) and
 *    restores the state of the figure that carries the same seat after a restart]
 *
 * WHY A SYSTEM OF ITS OWN. The spawn computes the figure; a hub write in the same file is hub I/O
 * beside calculation (WRFL_ASE_SYN_PATTERN). The seat rides beside the request as a number, and this
 * system carries it onto the figure and to the star and computes nothing.
 *
 * A REQUEST THAT BROUGHT NO FIGURE SEATS NOTHING. An answer without success files no value; the seat
 * row of the request goes with the pass all the same, so the request is read once.
 *
 * A SEAT PAST THE RANGE OF PLR_SEAT IS REFUSED, NEVER ROUNDED: PLR_SEAT is a float hub value, which
 * carries a whole number exactly only below PLR_HUB_SLOT_LIMIT (2^24, ARCH_ASE_CODEGEN_NET 7.6).
 *
 * HUB Pattern (ARCH_ASE_HUB_HS)
 *
 * READS (from Hub):   none
 * WRITES (to Hub):    "PLR_SEAT"_hs, owner = the figure
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
#include <ase/player/systems/hub/player_hub_slot_pub_sys.hpp>
// Components from same module
#include <ase/player/components/request/player_req_slot_comp.hpp>
#include <ase/player/components/request/player_req_spwn_res_comp.hpp>
#include <ase/player/components/state/player_sta_slot_comp.hpp>
#include <ase/player/components/tag/player_slot_pnd_tag.hpp>
#include <ase/player/types.hpp>
// Hub for the per-entity value
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>
// Range predicates (L0)
#include <ase/types/types.hpp>

#include <cstdint>
#include <entt/core/hashed_string.hpp>

namespace ase::player {
using namespace entt::literals;

/**
 * Anonymous namespace for PURE helper functions ONLY!
 * NO Views, NO Queries, NO Registry access in helpers!
 */
namespace {

// EMPTY ON PURPOSE: laying a row and filing a hub value write the registry; that is no pure helper.

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerHubSlotPubSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubSlotPubSystem] Started");
}

void PlayerHubSlotPubSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * PASS: every answered request that carries a seat. The cursor tag is no part of the view's
     * filter, and the figure the seat row lands on is another entity than the request, so neither
     * write changes the walked set.
     */
    for (auto [request, slot, answer] : registry.view<PlayerReqSlotComponent, PlayerReqSpwnResComponent>().each()) {
        registry.emplace_or_replace<PlayerSlotPndTag>(request);
        if (!answer.success) {
            continue;
        }
        const auto figure = static_cast<uint32_t>(answer.spawned_entity);
        if (!registry.valid(answer.spawned_entity)) {
            log::error(log::ERR::CAT::INVALID_ENTITY, "PlayerHubSlotPubSystem", figure);
            continue;
        }
        if (!ase::types::is_in_rng_u32(slot.seat, PLR_HUB_SLOT_MIN, PLR_HUB_SLOT_LIMIT - 1u)) {
            log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "PlayerHubSlotPubSystem", figure, "PLR_SEAT",
                      static_cast<float>(slot.seat));
            continue;
        }
        registry.emplace_or_replace<PlayerStaSlotComponent>(answer.spawned_entity).seat = slot.seat;
        hub::set(registry, figure, "PLR_SEAT"_hs, static_cast<float>(slot.seat));
        log::info("[PlayerHubSlotPubSystem] figure {} sits on seat {}", figure, slot.seat);
    }

    /* The seat row leaves every request this pass read, and the cursor ends with the pass. */
    auto read = registry.view<PlayerSlotPndTag>();
    registry.remove<PlayerReqSlotComponent>(read.begin(), read.end());
    registry.clear<PlayerSlotPndTag>();
}

void PlayerHubSlotPubSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubSlotPubSystem] Stopped");
}

}  // namespace ase::player
