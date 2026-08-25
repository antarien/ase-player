/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_hub_sess_reg_sys.cpp
 * @brief       PlayerHubSessRegSystem - Publish the live session index register on the Hub
 * @description Maintains PLR_ACTIVE_COUNT (GLOBAL) and PLR_OWNER per index slot so consumers can
 *              discover live players through the Hub, which has no iteration API.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    hub
 * @schedule    Dynamics
 * @created     2026-08-06
 * @modified    2026-08-06
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_HUB_SESS_REG: Session Index Register Publishing)
 *
 *   [PlayerStaIdntComponent + PlayerStaPosComponent]
 *          │
 *          │ the live player entities of this tier
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerHubSessRegSystem        │
 *   │                                             │
 *   │  READS:                                     │
 *   │    → PlayerStaIdntComponent (identity)         │
 *   │    → PlayerStaPosComponent (position)        │
 *   │    → "PLR_ACTIVE_COUNT"_hs (previous count) │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    → "PLR_OWNER"_hs (Hub, per slot owner)   │
 *   │    → "PLR_ACTIVE_COUNT"_hs (Hub, GLOBAL)    │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ live players discoverable without an ase-player dependency
 *          ▼
 *   TerrainStrmObsSyncSystem, GisColSessSynSystem, world_player_routes, predator logging
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1)):
 *
 * READS (from player module via Components):
 *   PlayerStaIdntComponent  → Player identity for iteration
 *   PlayerStaPosComponent → Only live, positioned players belong in the register
 *
 * READS (from Hub):
 *   "PLR_ACTIVE_COUNT"_hs → previous count, so the summary line is emitted on CHANGE only
 *
 * WRITES (to Hub for other modules):
 *   "PLR_OWNER"_hs        → player entity id of slot i, owner = hash of "PLR_ACTIVE_<i>"
 *   "PLR_ACTIVE_COUNT"_hs → number of live slots (GLOBAL)
 *
 * The entity id is published exactly as PlayerHubPosSystem scopes its own writes
 * (static_cast<uint32_t>(entity)), so a consumer that resolves PLR_OWNER can read PLR_POS_X/Y/Z
 * at that owner. Both cross the Hub as float32; entity ids stay far below the 2^24 mantissa
 * limit, so the round trip is exact.
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
#include <ase/player/systems/hub/player_hub_sess_reg_sys.hpp>
// Components from same module
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
// Module constants
#include <ase/player/types.hpp>
// Hub for O(1) API
#include <ase/hub/api.hpp>
// NOT_FOUND sentinel
#include <ase/types/types.hpp>
// Logging
#include <ase/log/log.hpp>
// Slot key hashing
#include <entt/core/hashed_string.hpp>

#include <cstdint>

namespace ase::player {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

/**
 * Build the Hub owner of one session index slot: the hash of "PLR_ACTIVE_<slot>".
 *
 * The key is assembled character by character because a module may not use std::string; the
 * digits fall out least significant first and are copied back in reverse. This MUST stay
 * byte-identical to the consumer side (terrain_strm_obs_sync_sys.cpp, world_player_routes.cpp),
 * because the two sides only ever meet in the resulting hash.
 */
uint32_t session_slot_owner(uint32_t slot) {
    char key[PLR_SESS_KEY_MAX] = {};
    const char prefix[] = "PLR_ACTIVE_";

    uint32_t len = 0;
    while (prefix[len] != '\0' && len + 1u < PLR_SESS_KEY_MAX) {
        key[len] = prefix[len];
        ++len;
    }

    char digits[PLR_SESS_DIGIT_MAX] = {};
    uint32_t digit_count = 0;
    uint32_t rest = slot;
    do {
        digits[digit_count] = static_cast<char>('0' + static_cast<char>(rest % 10u));
        ++digit_count;
        rest /= 10u;
    } while (rest != 0u && digit_count < PLR_SESS_DIGIT_MAX);

    while (digit_count > 0u && len + 1u < PLR_SESS_KEY_MAX) {
        --digit_count;
        key[len] = digits[digit_count];
        ++len;
    }
    key[len] = '\0';

    return entt::hashed_string{key}.value();
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerHubSessRegSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubSessRegSystem] Started");
}

void PlayerHubSessRegSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Read the previous count and validate it IMMEDIATELY.
     * The system is stateless, so the Hub itself carries the comparison value. A missing key is
     * not an error here - it is the very first tick of this tier, and the write below creates it.
     */
    const float prev_count_f = hub::get(registry, hub::GLOBAL, "PLR_ACTIVE_COUNT"_hs);
    const bool  had_previous = !types::is_not_found(prev_count_f);
    const uint32_t prev_count = had_previous ? static_cast<uint32_t>(prev_count_f) : 0u;

    /**
     * STEP 2: Walk the live players and stamp one index slot each.
     * Views are created on demand, not stored as member variables. PlayerStaPosComponent is part of
     * the view because a player without a position cannot serve any consumer of this register -
     * every one of them resolves the slot to PLR_POS_X/Y/Z.
     */
    auto view = registry.view<PlayerStaIdntComponent, PlayerStaPosComponent>();

    uint32_t slot = 0;
    for (auto [entity, id, pos] : view.each()) {
        if (slot >= PLR_SESS_SLOT_MAX) {
            log::warn(log::WRN::CAT::CAPACITY_REACHED, "PlayerHubSessRegSystem",
                      "session_slots", static_cast<float>(PLR_SESS_SLOT_MAX));
            break;
        }

        const uint32_t owner = static_cast<uint32_t>(entity);
        /**
         * SABOTAGE-BESTAND (Betreiber 2026-08-10): Identitaet im Hub-float - muss behoben werden.
         * Register: docs/ase-docs/tech/servers/plans/compute/audits/05-TASKREGISTER.md:1297.
         *
         * PLR_OWNER traegt eine Entitaetsnummer in einem f32-Werteslot. Eine Identitaet ist kein
         * Wert in einem Bereich: die 12 entt-Versionsbits heben den u32 ueber die 2^24-Mantisse,
         * als Zahl gelesen kommt die Nummer GERUNDET zurueck (live 2026-08-10 21:44 auf
         * world-9102: "session adoption refused: ... pos_miss=1"). Die legitimen Kanaele sind
         * Tag, Konstante oder Telemetrie-Laufzeitwert; Hashes reisen ueber Almanach/Snapshot.
         * Der Terrain-Beobachtergang liest dieses Register seit dem Kanalentscheid NICHT mehr
         * (Tag-View ueber HubLifeAlivTag); der letzte Leser ist die Webserver-Route
         * world_player_routes.cpp, deren Almanach-Umbau im Register gefuehrt ist.
         */
        hub::set(registry, session_slot_owner(slot), "PLR_OWNER"_hs, static_cast<float>(owner));
        ++slot;
    }

    /**
     * STEP 3: Publish the count LAST.
     * Consumers walk slots below the count, so the slots exist before the count admits them.
     * Writing every tick keeps the register self-healing: a despawned player lowers the count on
     * the next tick, and no start order can leave a stale value behind.
     */
    hub::set(registry, hub::GLOBAL, "PLR_ACTIVE_COUNT"_hs, static_cast<float>(slot));

    /**
     * STEP 4: Report a CHANGE, never a steady state.
     * Without this line the register fails silently in exactly the way it did before it existed:
     * a consumer reads 0 and nobody can tell whether nothing was published or nothing is live.
     */
    if (!had_previous || prev_count != slot) {
        log::info("[PlayerHubSessRegSystem] session register: {} live slot(s) published", slot);
    }
}

void PlayerHubSessRegSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerHubSessRegSystem] Stopped");
}

}  // namespace ase::player
