/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_life_desp_sys.cpp
 * @brief       PlayerLifeDespSystem - Process player despawn requests and destroy the marked
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    ecs/entity/entitylifecycle
 * @schedule    Dynamics
 * @created     2026-08-30
 * @modified    2026-08-30
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_LIFE_DESP: the end of a player's life)
 *
 *   [PlayerReqDespComponent]
 *          │
 *          │ despawn requests from Integration Layer
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerLifeDespSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - PlayerReqDespComponent (requests)      │
 *   │    - PlayerStaIdntComponent (who is meant)  │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - PlayerDespPndTag (the pending mark)    │
 *   │    - PlayerReqDespResComponent (result)     │
 *   │    - destroys the marked entities           │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ the player row is gone
 *          ▼
 *   Observer systems in other modules lose their anchor and clean up
 *
 * WHY THIS IS ITS OWN SYSTEM (split from PlayerLifeSpwnSystem, 2026-08-30)
 *
 * The origin answered both request kinds from one tick and sat in the unsplit band. Birth and
 * death read different request components, write different results and share nothing but the
 * identity index. The split is by responsibility; the file size followed, it did not lead.
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   (none - a despawn needs no world value; the request carries the identity)
 *
 * WRITES (to Hub for other modules):
 *   (none - observer systems see the entity disappear)
 *
 * NOTE: Deferred deletion. The request loop only MARKS; the destruction runs after it, over the
 * mark. Destroying inside the request loop would invalidate the view it iterates.
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
#include <ase/player/systems/lifecycle/player_life_desp_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/request/player_req_desp_comp.hpp>
#include <ase/player/components/request/player_req_desp_res_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/tag/player_desp_pnd_tag.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Containers SSOT - der Spieler-Index in tick() (NO std:: maps)
#include <ase/containers/hash_map.hpp>
// Logging
#include <ase/log/log.hpp>

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

// No helper functions needed - the identity hash is three lines and lives where it is used. A
// helper taking the Registry would be a system without a file.

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerLifeDespSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerLifeDespSystem] Started");
}

void PlayerLifeDespSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * DER SPIELER-INDEX WIRD EINMAL JE PASS GEBAUT, NICHT EINMAL JE ANFRAGE.
     *
     * Die Frage lautet "gibt es schon eine Zeile mit dieser player_id?" und wurde bis 2026-08-18
     * mit einer vollstaendigen view<PlayerStaIdntComponent> INNERHALB der Anfrage-Schleife
     * beantwortet: M Anfragen mal N Spieler. Der Aufwand sah billig aus, weil die Sicht in einer
     * Zeile stand.
     *
     * SEIT DEM SCHNITT 2026-08-30 BAUT JEDES DER BEIDEN SYSTEME SEINEN EIGENEN INDEX - einmal je
     * Takt, O(N). Das ist NICHT der Rueckfall in den alten Defekt: der lag in der SCHLEIFE, nicht
     * in der Zahl der Systeme. Eine geteilte Karte muesste in einem Component liegen, und eine
     * HashMap ist kein POD.
     *
     * Der Index ist ein VORFILTER, keine Antwort. Er fuehrt Hash auf Entity; wer einen Treffer
     * hat, vergleicht die Kennung an der Fundstelle nach. Ohne diese Nachpruefung waere eine
     * Hash-Kollision keine ausbleibende, sondern eine PLAUSIBEL FALSCHE Antwort - der falsche
     * Spieler, still geloescht. Mit der Nachpruefung ist der schlimmste Fall ein "nicht gefunden".
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
     * STEP 1: Process local despawn requests (use deferred deletion)
     */
    auto desp_view = registry.view<PlayerReqDespComponent>();
    auto desp_it = desp_view.begin();
    auto desp_end = desp_view.end();
    while (desp_it != desp_end) {
        auto request_entity = *desp_it;
        ++desp_it;

        auto& request = registry.get<PlayerReqDespComponent>(request_entity);
        bool success = false;

        // Find and mark player for deletion - O(1) gegen den Index, Kennung an der
        // Fundstelle geprueft.
        uint64_t desp_hash = PLR_ID_FNV_OFFSET;
        for (uint32_t i = 0;
             i < static_cast<uint32_t>(sizeof(request.player_id)) - 1u
             && request.player_id[i] != '\0';
             ++i) {
            desp_hash = (desp_hash ^ static_cast<uint8_t>(request.player_id[i])) * PLR_ID_FNV_PRIME;
        }

        auto desp_hit = player_by_id.find(desp_hash);
        if (desp_hit != player_by_id.end()) {
            const auto* known = registry.try_get<PlayerStaIdntComponent>(desp_hit->second);
            if (known != nullptr &&
                std::strncmp(known->player_id, request.player_id,
                             sizeof(known->player_id) - 1) == 0) {
                registry.emplace_or_replace<PlayerDespPndTag>(desp_hit->second);
                success = true;
                log::debug("[PlayerLifeDespSystem] Marked player for despawn");
            }
        }

        if (!success) {
            log::debug("[PlayerLifeDespSystem] Player not found for despawn");
        }

        auto& result = registry.emplace<PlayerReqDespResComponent>(request_entity);
        result.success = success;

        registry.remove<PlayerReqDespComponent>(request_entity);
    }

    /**
     * STEP 2: Destroy entities tagged for despawn (deferred deletion)
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

void PlayerLifeDespSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerLifeDespSystem] Stopped");
}

}  // namespace ase::player
