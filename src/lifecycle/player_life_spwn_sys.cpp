/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_life_spwn_sys.cpp
 * @brief       PlayerLifeSpwnSystem - Process player spawn requests and create the player row
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    ecs/entity/entitylifecycle
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-08-30
 * @version     2.0.0
 *
 * WHAT THIS FILE KEPT AND WHAT LEFT IT (split 2026-08-30)
 *
 * This file carried 518 lines and THREE concerns: it created players, it set walkers on their
 * errand, and it destroyed players. Birth and death are two lifecycles, not two paragraphs of
 * one - they read different request components, write different results and share nothing but
 * the identity index. The errand named itself a concern in this file's own words ("this is the
 * ONE place where a requested speed meets the movement authority") and owns its own request
 * component, state component, two tags, warning and log line.
 *
 *   the errand    → PlayerLifeRoamIniSystem (player_life_roam_ini_sys.cpp)
 *   despawn       → PlayerLifeDespSystem    (player_life_desp_sys.cpp)
 *
 * THE NAME STAYED because what is left is the spawn: this file answers PlayerReqSpwnComponent
 * and creates the player row. Both new systems run AFTER it in Dynamics, the same sequence the
 * single system had.
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
 *   │    - PlayerReqSpwnComponent (requests)      │
 *   │    - PlayerStaIdntComponent (existing check) │
 *   │    - "TRN_HGT_AT_POS"_hs (Hub - height)     │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - PlayerStaIdntComponent (create)        │
 *   │    - PlayerStaPosComponent (create)         │
 *   │    - PlayerStaYawComponent (create)         │
 *   │    - PlayerStaVelComponent (create)         │
 *   │    - PlayerStaPhysComponent (create)        │
 *   │    - PlayerStaStsComponent (create)         │
 *   │    - PlayerStaChkComponent (create)         │
 *   │    - hub::HubPlrSpndTag (create)            │
 *   │    - hub::HubLifeAlivTag (create)           │
 *   │    - PlayerDrtyTag (create)                 │
 *   │    - PlayerReqSpwnResComponent (result)     │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ player entities created with the spawn and life marks
 *          ▼
 *   PlayerLifeRoamIniSystem reads the result and sets a walker on its errand;
 *   observer systems in other modules add their components
 *   (InputSpawnObserver, CameraSpawnObserver, etc.)
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   "TRN_HGT_AT_POS"_hs → Terrain height at position (set by terrain module)
 *
 * WRITES (to Hub for other modules):
 *   (none - observer systems read the spawn mark directly)
 *
 * NOTE: This system ONLY creates player components. Other modules observe the spawn mark and add
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
// types.hpp for constants
#include <ase/player/types.hpp>
// Hub for HUB Pattern (cross-module reads) - und seit 2026-08-18 auch die Lebensmarke
// hub::HubLifeAlivTag, auf die die Beobachter-Adoption filtert (TerrainStrmObsSyncSystem PASS 2,
// Betreiber-Kanalentscheid 2026-08-10). Sie kam vorher als POD-Header ueber den Include-Pfad aus
// ase-lifecycle - genau die L3-auf-L3-Kopplung, die der Umzug aufloest.
#include <ase/hub/api.hpp>
// Containers SSOT - der Spieler-Index in tick() (NO std:: maps)
#include <ase/containers/hash_map.hpp>
// Types SSOT (L0) - die Sentinel-Praedikate fuer den Hub-Rueckgabewert beim Spawn
#include <ase/types/types.hpp>
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

// No helper functions needed - all logic inlined in system methods. The draw sequence a walker's
// first pause and first course come from moved out with the errand and now lives entirely in
// PlayerLifeRoamIniSystem; it is still ONE statement in types.hpp (plr_roam_fraction,
// plr_roam_span), shared with the two phase systems.

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
         * The manager entity used to carry PlayerStMovComponent, filled with twelve
         * MOVEMENT_DEFAULT_* constants (2026-08-15 removed). Every consumer read the
         * same constants back, so the component was a per-entity duplicate of
         * types.hpp - forbidden by CLAUDE.md ("'Config-Components' with constants =
         * FORBIDDEN"). The manager tag itself stays: it marks the singleton entity.
         */
        log::info("[PlayerLifeSpwnSystem] Created player manager entity");
    }
}

void PlayerLifeSpwnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Movement settings come straight from types.hpp (2026-08-15).
     * They used to be copied into PlayerStMovComponent and read back through a
     * manager lookup; every field only ever held a MOVEMENT_DEFAULT_* constant.
     */

    /**
     * DER SPIELER-INDEX WIRD EINMAL JE PASS GEBAUT, NICHT EINMAL JE ANFRAGE.
     *
     * Die Frage lautet "gibt es schon eine Zeile mit dieser player_id?" und wurde bis 2026-08-18
     * mit einer vollstaendigen view<PlayerStaIdntComponent> INNERHALB der Anfrage-Schleife
     * beantwortet: M Anfragen mal N Spieler, und das zweimal im selben tick, weil die
     * Despawn-Schleife dieselbe Frage stellte. Der Aufwand sah billig aus, weil die Sicht in
     * einer Zeile stand.
     *
     * SEIT DEM SCHNITT 2026-08-30 STEHT DIE DESPAWN-SCHLEIFE IN PlayerLifeDespSystem und baut
     * dort ihren eigenen Index - einmal je Takt, O(N). Das ist NICHT der Rueckfall in den alten
     * Defekt: der lag in der SCHLEIFE, nicht in der Zahl der Systeme. Eine geteilte Karte
     * muesste in einem Component liegen, und eine HashMap ist kein POD.
     *
     * Der Index ist ein VORFILTER, keine Antwort. Er fuehrt Hash auf Entity; wer einen Treffer
     * hat, vergleicht die Kennung an der Fundstelle nach. Ohne diese Nachpruefung waere eine
     * Hash-Kollision keine ausbleibende, sondern eine PLAUSIBEL FALSCHE Antwort - der falsche
     * Spieler, still gefunden. Mit der Nachpruefung ist der schlimmste Fall ein "nicht gefunden".
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

        // Check if player already exists - O(1) gegen den Index, Kennung an der Fundstelle geprueft
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
            // Get terrain height via Hub (HUB Pattern - READS)
            float ground_y = 0.0f;
            uint32_t pos_hash = static_cast<uint32_t>(
                static_cast<int32_t>(request.x) * 73856093 ^
                static_cast<int32_t>(request.z) * 19349663);
            /**
             * ABWESENHEIT WIRD GEFRAGT, NICHT VERGLICHEN (2026-08-20)
             *
             * Hier stand `hub_height != hub::NOT_FOUND`. Das Sentinel ist ein BEREICH
             * (`v <= FloatNotFound`, types.hpp:65), kein einzelner Wert. Dieselbe Umstellung ist
             * am 2026-08-19 in zehn ase-combat-Systemen gefahren worden; die Begruendung steht
             * dort ausfuehrlich. Die zweite Kopie dieser Stelle liegt in
             * character_life_spwn_sys.cpp und ist am selben Tag mitgegangen.
             *
             * `is_val_float` und nicht `!is_not_found`, weil der Wert eine KOORDINATE wird: die
             * Verneinung von is_not_found laesst den UNSET-Sentinel durch und haette die Figur
             * in dessen Hoehe gesetzt statt auf den Boden. is_val_float schliesst beide Enden
             * aus (`v > FloatNotFound && v < FloatUnset`, types.hpp:61).
             *
             * Der Block steht VOR der Lesung und nicht zwischen Lesung und Pruefung: die Regel
             * HUB_WITHOUT_CHECK sucht die Pruefung in einem Fenster von fuenf Zeilen hinter
             * `hub::get` (11_module_layer.json, `_note_window`). Ein Kommentar dazwischen
             * schiebt sie aus dem Fenster, und der Befund kehrt zurueck.
             *
             * Bleibt der Wert ungueltig, spawnt die Figur auf y=0 — das Terrain-Modul zieht die
             * Hoehe nach, sobald der Chunk geladen ist.
             */
            float hub_height = hub::get(registry, pos_hash, "TRN_HGT_AT_POS"_hs);
            if (ase::types::is_val_float(hub_height)) {
                ground_y = hub_height;
            }

            // Create player entity with ONLY player components
            result_entity = registry.create();

            auto& identity = registry.emplace<PlayerStaIdntComponent>(result_entity);
            std::strncpy(identity.player_id, request.player_id, sizeof(identity.player_id) - 1);
            identity.player_id[sizeof(identity.player_id) - 1] = '\0';

            /**
             * DER INDEX WIRD MITGEFUEHRT, nicht nur gelesen. Zwei Anfragen mit derselben Kennung
             * im selben Pass haetten sonst zwei Spieler erzeugt - und seit dem Schnitt gilt das
             * doppelt: PlayerLifeDespSystem baut seinen Index NACH diesem Pass, sieht den frisch
             * angelegten Spieler also ohnehin, aber innerhalb DIESER Schleife bleibt die
             * Mitfuehrung die einzige Abwehr gegen die doppelte Anlage.
             */
            player_by_id.emplace(req_hash, result_entity);

            // Timestamps initialized to 0 - can be set via Hub or request if needed
            identity.spawned_at_ms = 0;
            identity.last_input_ms = 0;

            /**
             * CHUNK-RELATIV (S2b 2026-08-11): der Draht liefert Weltmeter (f32), gefuehrt wird
             * die Wabenadresse exakt plus kleine lokale Meter - an Ort 7 (204 Mio Weltmeter,
             * ULP 16 m) verpuffte sonst jeder Schritt (Muster character_life_spwn_sys.cpp).
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

            // Lifecycle tags - observer systems in other modules will see these
            // and add their own components (input, camera, terrain streaming)
            registry.emplace<hub::HubPlrSpndTag>(result_entity);
            registry.emplace<PlayerDrtyTag>(result_entity);

            /**
             * DIE LEBENSMARKE - ein Siedler LEBT, und wer lebt, wird beobachtet: die
             * Beobachter-Adoption (TerrainStrmObsSyncSystem PASS 2) sieht ausschliesslich
             * hub::HubLifeAlivTag (Betreiber-Kanalentscheid 2026-08-10). Ohne die Marke
             * wanderte der Siedler unsichtbar - 381 Wabenuebertritte, null Zellmarkierungen
             * (gemessen 2026-08-11). Seine Seele fuehrt die SITZUNG (MS/PC): BdiAgtRegSystem
             * excluded player::PlayerSpndTag, damit hier kein zweiter Wille entsteht.
             */
            registry.emplace<hub::HubLifeAlivTag>(result_entity);

            success = true;
            log::debug("[PlayerLifeSpwnSystem] Spawned player");
        } else {
            log::debug("[PlayerLifeSpwnSystem] Player already exists");
        }

        /**
         * DAS ERGEBNIS IST DIE NAHT ZUM AUFTRAG (Schnitt 2026-08-30)
         *
         * PlayerLifeRoamIniSystem liest genau dieses Paar - Ergebnis plus der
         * PlayerReqRoamComponent, der mit der Anfrage mitreiste - und setzt den Wanderer auf
         * seinen Weg. Dieses System fasst den Auftrag NICHT mehr an und entfernt ihn NICHT: er
         * wird dort verzehrt, ein System spaeter im selben Takt.
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
