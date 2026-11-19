/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_chr_pub_sys.cpp
 * @brief       PlayerHubChrPubSystem - tells the star which character a new figure plays
 * @description Files one row of kind HUB_CRT_CHR for every answered spawn request that names a
 *              character.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101557 - receive a spawn request
 * @actor       Player Presence Steward (PLAYPRESTE)
 * @category    hub
 * @schedule    Dynamics
 * @parity      server_only
 * @created     2026-11-17
 * @modified    2026-11-17
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (CAUSA_ASE_CREATION, which character a figure plays)
 *
 *   [PlayerHubSpwnSystem laid PlayerReqChrComponent on a spawn request whose row names a
 *    character; PlayerLifeSpwnSystem answered the request with PlayerReqSpwnResComponent]
 *          │
 *          ▼
 *   ┌───────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubChrPubSystem                           │
 *   │  READS:  PlayerReqChrComponent, PlayerReqSpwnResComponent     │
 *   │  WRITES: HUB_CRT_CHR (Hub row: figure, hash of the character) │
 *   └───────────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   [ase-creation takes the row and files the genome and the start values of the character]
 *
 * WHY A SYSTEM OF ITS OWN. The spawn computes the figure - its place, its chunk address - and a
 * hub write in the same file is hub I/O beside calculation (WRFL_ASE_SYN_PATTERN). The spawn
 * states its answer on the request; this system carries the character to the star and computes
 * nothing.
 *
 * A REQUEST THAT BROUGHT NO FIGURE NAMES NO CHARACTER. An answer without success files no row; the
 * character row of the request goes with the pass all the same, so the request is read once.
 *
 * HUB Pattern (ARCH_ASE_HUB_API, typed rows)
 *
 * READS (from Hub):   none
 * WRITES (to Hub):    "HUB_CRT_CHR_BODY"_hs, "HUB_CRT_CHR_KEY"_hs under the mark HUB_CRT_CHR_RGST
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
#include <ase/player/systems/hub/player_hub_chr_pub_sys.hpp>
// Components from same module
#include <ase/player/components/request/player_req_chr_comp.hpp>
#include <ase/player/components/request/player_req_spwn_res_comp.hpp>
#include <ase/player/components/tag/player_chr_pnd_tag.hpp>
// Hub for the row API
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>
#include <entt/core/hashed_string.hpp>

namespace ase::player {
using namespace entt::literals;

/**
 * Anonymous namespace for PURE helper functions ONLY!
 * NO Views, NO Queries, NO Registry access in helpers!
 */
namespace {

// EMPTY ON PURPOSE: filing a row writes the hub; that is no pure helper.

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerHubChrPubSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubChrPubSystem] Started");
}

void PlayerHubChrPubSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * PASS: every answered request that names a character. The cursor tag is no part of the
     * view's filter, so stamping inside the walk leaves the walked set as it is.
     */
    for (auto [request, chr, answer] : registry.view<PlayerReqChrComponent, PlayerReqSpwnResComponent>().each()) {
        registry.emplace_or_replace<PlayerChrPndTag>(request);
        if (!answer.success) {
            continue;
        }
        const auto figure = static_cast<uint32_t>(answer.spawned_entity);
        const ecs::Entity row_entity = hub::row_create(registry, "HUB_CRT_CHR"_hs);
        const auto row = static_cast<uint32_t>(row_entity);
        hub::set_u32(registry, row, "HUB_CRT_CHR_BODY"_hs, figure);
        hub::set_u32(registry, row, "HUB_CRT_CHR_KEY"_hs, chr.character_hash);
        hub::row_tag(registry, row_entity, "HUB_CRT_CHR_RGST"_hs);
        log::info("[PlayerHubChrPubSystem] figure {} plays the character {}", figure, chr.character_hash);
    }

    /* The character row leaves every request this pass read, and the cursor ends with the pass. */
    auto read = registry.view<PlayerChrPndTag>();
    registry.remove<PlayerReqChrComponent>(read.begin(), read.end());
    registry.clear<PlayerChrPndTag>();
}

void PlayerHubChrPubSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubChrPubSystem] Stopped");
}

}  // namespace ase::player
