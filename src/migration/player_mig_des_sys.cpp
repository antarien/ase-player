/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_mig_des_sys.cpp
 * @brief       PlayerMigDesSystem - deserialize migrated-in PlayerSnaps and honour migrate acks
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    entity/actor/player
 * @schedule    Reception
 * @created     2026-08-19
 * @modified    2026-08-19
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Star deliveries → instantiated figures and released source copies)
 *
 *   [ase-world filed deliveries: a frozen PlayerSnap to unpack, or an ack to honour]
 *          │
 *          │ HubPlrMigSnapComponent / HubPlrMigAckComponent + HubPlrMigPndTag
 *          ▼
 *   ┌───────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerMigDesSystem                          │
 *   │                                                           │
 *   │  READS:                                                   │
 *   │    - hub::HubPlrMigSnapComponent (snap, snap_len)         │
 *   │    - hub::HubPlrMigAckComponent (player_ref, ack_epoch)   │
 *   │    - PlayerStaEpchComponent (O(1) ref lookup, pre-pass)   │
 *   │    - PlayerStaIdntComponent (despawn identity, OWN row)   │
 *   │                                                           │
 *   │  WRITES:                                                  │
 *   │    - PlayerStaEpch/Pos/Yaw/Vel/Sts/Chk + PlayerSpndTag    │
 *   │      (instantiate or in-place update, OWN types)          │
 *   │    - PlayerReqDespComponent (module-own despawn request)  │
 *   │    - hub::HubPlrMigFinTag (receipt on the delivery)       │
 *   │    - delivery rows consumed (remove) after the loops      │
 *   └───────────────────────────────────────────────────────────┘
 *          │
 *          │ the figure runs on this tier; the empty hull carries only the receipt
 *          ▼
 *   [ase-world retires the Fin-marked hulls it created - batch destroy, its entities]
 *
 * WARUM ES DIESES SYSTEM GIBT. Bis 2026-08-19 packte ase-world das eingefrorene 42-Byte-Bild
 * SELBST aus und schrieb sieben player-Zeilen (world_plr_mig_rcv_sys.cpp) — serialisieren tat
 * der Eigentuemer (PlayerMigSerSystem), deserialisieren der Fremde. Diese Asymmetrie WAR die
 * verbotene Kante ase-world → ase-player. Jetzt entpackt der Eigentuemer; ase-world stellt nur
 * noch zu und ruehrt keine player-Zeile mehr an.
 *
 * DIE STA-CHK-ZEILE WIRD ANGELEGT, NICHT GEFUELLT. PlayerStaChnkSystem (Dynamics) fuehrt sie
 * aus der Positionszeile nach und veroeffentlicht PLR_CHK_X/Y am Stern — eine zweite
 * Wabenformel hier waere eine zweite Vergabestelle derselben Ableitung. Der alte world-Code
 * fuellte sie mit der WELT-Wabenkonstante und lief damit genau in diese Doppelung.
 *
 * DELIVERY-LEBENSLAUF (WRFL_ASE_DEFERRED_DELETION). Die Drains filtern auf HubPlrMigPndTag und
 * duerfen ihn deshalb nicht anfassen; sie setzen nur HubPlrMigFinTag, das in keiner ihrer
 * Signaturen steht. Der Verbrauchs-Pass laeuft danach ueber Fin und nimmt die Zeilen + Pnd ab
 * (Typen ausserhalb SEINER Signatur). Die leere Huelle traegt nur noch Fin; zerstoert wird sie
 * von ihrem ERZEUGER ase-world per Batch — wer die Entity anlegt, raeumt sie ab.
 *
 * HUB Pattern (ARCH_ASE_HUB_API v2.0)
 *
 * READS (from Hub): HubPlrMigSnapComponent, HubPlrMigAckComponent (deliveries, via mirror tag)
 * WRITES (to Hub): HubPlrMigFinTag (receipt). Keine hub::set/get-Werte: die Naht laeuft ueber
 *                  Zustellungen, nicht ueber stehende Slots (ARCH_ASE_HUB_API.md:111-123).
 *
 * FLYWEIGHT PATTERN (Not used - the 42-byte image travels inline, no resource handle needed)
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
#include <ase/player/systems/migration/player_mig_des_sys.hpp>
// Frozen PlayerSnap wire layout (offsets, schema version, size) - L0 contract SSOT
#include <ase/types/region_wire.hpp>
// The star deliveries of the migrate seam (rows + markers) - via the ONE allowed hub entry
#include <ase/hub/api.hpp>
// Own state rows the figure is built from
#include <ase/player/components/state/player_sta_epch_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_yaw_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_sts_comp.hpp>
#include <ase/player/components/state/player_sta_chk_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
// Own despawn request and the embodiment marker
#include <ase/player/components/request/player_req_desp_comp.hpp>
#include <ase/player/components/tag/player_spnd_tag.hpp>
// types.hpp for constants (chunk edge for the world-metre split)
#include <ase/player/types.hpp>
// O(1) row index for the drain loops (SSOT HashMap, no std:: container) - L3→L0, allowed
#include <ase/containers/hash_map.hpp>
// Math (floor from L0 - world metres → chunk coordinate)
#include <ase/math/math.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>

namespace ase::player {
using namespace entt::literals;

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO View/Query in helpers! (pure binary deserialization only)
 */
namespace {

// Decode one frozen PlayerSnap (little-endian) from a delivery buffer. The caller guarantees
// the buffer holds types::PLAYER_SNAP_SZ bytes. Mirror of encode_player_snap in
// player_mig_ser_sys.cpp - same offsets, same L0 SSOT. Pure: memcpy only.
void decode_player_snap(const char* snap, uint16_t& schema, uint32_t& player_ref,
                        uint32_t& player_epoch, float& x, float& y, float& z, float& yaw,
                        float& vx, float& vy, float& vz, uint32_t& status) {
    std::memcpy(&schema, snap + types::PLAYER_SNAP_OFF_SCHEMA, 2);
    std::memcpy(&player_ref, snap + types::PLAYER_SNAP_OFF_ID, 4);
    std::memcpy(&player_epoch, snap + types::PLAYER_SNAP_OFF_EPOCH, 4);
    std::memcpy(&x, snap + types::PLAYER_SNAP_OFF_POS, 4);
    std::memcpy(&y, snap + types::PLAYER_SNAP_OFF_POS + 4u, 4);
    std::memcpy(&z, snap + types::PLAYER_SNAP_OFF_POS + 8u, 4);
    std::memcpy(&yaw, snap + types::PLAYER_SNAP_OFF_POS + 12u, 4);
    std::memcpy(&vx, snap + types::PLAYER_SNAP_OFF_VEL, 4);
    std::memcpy(&vy, snap + types::PLAYER_SNAP_OFF_VEL + 4u, 4);
    std::memcpy(&vz, snap + types::PLAYER_SNAP_OFF_VEL + 8u, 4);
    std::memcpy(&status, snap + types::PLAYER_SNAP_OFF_STATUS, 4);
}

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerMigDesSystem::on_start(ecs::Registry& registry) {
    log::debug("[PlayerMigDesSystem] Started");
    (void)registry;
}

void PlayerMigDesSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * FRUEHAUSSTIEG - keine anstehende Zustellung, kein Index-Aufbau. Der Vorpass unten kostet
     * einen Lauf ueber alle Figuren; er lohnt nur, wenn es etwas zuzustellen gibt.
     */
    auto snap_view = registry.view<hub::HubPlrMigSnapComponent, hub::HubPlrMigPndTag>();
    auto ack_view = registry.view<hub::HubPlrMigAckComponent, hub::HubPlrMigPndTag>();
    if (snap_view.begin() == snap_view.end() && ack_view.begin() == ack_view.end()) {
        return;
    }

    /**
     * VORPASS - O(1)-Index player_ref → Figur. Der alte world-Empfaenger lief je Frame linear
     * ueber alle Epochenzeilen (die Setz-Richtung des M:N); der Index faellt einmal je Takt an
     * und traegt alle Zustellungen dieses Takts.
     */
    containers::HashMap<uint32_t, ecs::Entity> plr_by_ref;
    for (auto [plr_ent, epch] : registry.view<PlayerStaEpchComponent>().each()) {
        if (epch.player_ref != 0u) {
            plr_by_ref[epch.player_ref] = plr_ent;
        }
    }

    /**
     * PASS A - Snap-Zustellungen: epoch-idempotentes Anlegen bzw. In-Place-Update.
     *
     * Die Walk filtert auf HubPlrMigPndTag und ruehrt ihn nicht an; sie setzt nur die Quittung
     * HubPlrMigFinTag, die in ihrer Signatur nicht vorkommt. Auch VERWORFENE Zustellungen
     * (falsche Laenge, falsches Schema, Duplikat) werden quittiert: ein halbes Bild ist von
     * einem vollstaendigen nicht zu unterscheiden, und eine unquittierte Zeile staut sich und
     * blockiert jede weitere Wanderung derselben Figur.
     */
    for (auto [dlv_ent, snap] : snap_view.each()) {
        if (snap.snap_len != types::PLAYER_SNAP_SZ) {
            // Der Wert existiert, ist aber unbrauchbar - gemeldet, BEVOR die Zeile faellt
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerMigDesSystem",
                      static_cast<uint32_t>(dlv_ent), "snap_len",
                      static_cast<float>(snap.snap_len));
            registry.emplace<hub::HubPlrMigFinTag>(dlv_ent);
            continue;
        }

        uint16_t schema = 0;
        uint32_t player_ref = 0;
        uint32_t snap_epoch = 0;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float yaw = 0.0f;
        float vx = 0.0f;
        float vy = 0.0f;
        float vz = 0.0f;
        uint32_t status = 0;
        decode_player_snap(snap.snap, schema, player_ref, snap_epoch,
                           x, y, z, yaw, vx, vy, vz, status);

        if (schema != types::PLAYER_SNAP_SCHEMA_VER) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerMigDesSystem",
                      static_cast<uint32_t>(dlv_ent), "snap_schema",
                      static_cast<float>(schema));
            registry.emplace<hub::HubPlrMigFinTag>(dlv_ent);
            continue;
        }

        // Epoch-Idempotenz: eine gehaltene Epoche >= der zugestellten ist ein Duplikat oder
        // ein verspaeteter Migrate - die Figur wird nie doppelt simuliert.
        ecs::Entity plr_ent = ecs::Entity{};
        bool plr_found = false;
        auto ref_hit = plr_by_ref.find(player_ref);
        if (ref_hit != plr_by_ref.end() && registry.valid(ref_hit->second)) {
            plr_ent = ref_hit->second;
            plr_found = true;
            const auto& held = registry.get<PlayerStaEpchComponent>(plr_ent);
            if (held.player_epoch >= snap_epoch) {
                log::info("[PlayerMigDes] duplicate/late migrate for player_ref={} (epoch {} <= held {}) - dropped",
                          player_ref, snap_epoch, held.player_epoch);
                registry.emplace<hub::HubPlrMigFinTag>(dlv_ent);
                continue;
            }
        }

        if (!plr_found) {
            plr_ent = registry.create();
            registry.emplace<PlayerStaEpchComponent>(plr_ent);
            registry.emplace<PlayerStaPosComponent>(plr_ent);
            registry.emplace<PlayerStaYawComponent>(plr_ent);
            registry.emplace<PlayerStaVelComponent>(plr_ent);
            registry.emplace<PlayerStaStsComponent>(plr_ent);
            // Angelegt, NICHT gefuellt: PlayerStaChnkSystem fuehrt die Wabenzeile aus der
            // Positionszeile nach und ist die EINE Vergabestelle dieser Ableitung.
            registry.emplace<PlayerStaChkComponent>(plr_ent);
            registry.emplace<PlayerSpndTag>(plr_ent);
            // Der STERN-Spiegel der Verkoerperung, im SELBEN Zug wie der Modul-Tag: Kamera und
            // bdi zaehlen ueber hub::HubPlrSpndTag auf (camera_tgt_cyc_sys.cpp:213). Der
            // Spawn-Pfad setzt ihn genauso (player_life_spwn_sys.cpp:364); eine zugewanderte
            // Figur ohne Spiegel waere fuer beide unsichtbar — ein halb gesetztes Paar, und
            // beide Tore blieben gruen.
            registry.emplace<hub::HubPlrSpndTag>(plr_ent);
            // Neue Figur in den Takt-Index, damit eine zweite Zustellung desselben Spielers im
            // SELBEN Takt als Update laeuft statt eine Doppel-Entity anzulegen.
            plr_by_ref[player_ref] = plr_ent;
        }

        auto& epch = registry.get<PlayerStaEpchComponent>(plr_ent);
        epch.player_ref = player_ref;
        epch.player_epoch = snap_epoch;
        /**
         * CHUNK-RELATIV (S2b 2026-08-11): der eingefrorene PlayerSnap liefert Weltmeter (f32);
         * gefuehrt wird die Wabenadresse exakt plus lokale Meter - floor/Rest wie beim Spawn
         * (player_life_spwn_sys.cpp), mit der EIGENEN Wabenkonstante dieses Moduls.
         */
        auto& pos = registry.get<PlayerStaPosComponent>(plr_ent);
        pos.chunk_x = static_cast<int32_t>(math::floor(x / MOVEMENT_DEFAULT_CHUNK_SIZE));
        pos.chunk_z = static_cast<int32_t>(math::floor(z / MOVEMENT_DEFAULT_CHUNK_SIZE));
        pos.local_x = x - static_cast<float>(pos.chunk_x) * MOVEMENT_DEFAULT_CHUNK_SIZE;
        pos.local_z = z - static_cast<float>(pos.chunk_z) * MOVEMENT_DEFAULT_CHUNK_SIZE;
        pos.y = y;
        auto& yaw_row = registry.get<PlayerStaYawComponent>(plr_ent);
        yaw_row.yaw = yaw;
        auto& vel = registry.get<PlayerStaVelComponent>(plr_ent);
        vel.vx = vx;
        vel.vy = vy;
        vel.vz = vz;
        auto& sts = registry.get<PlayerStaStsComponent>(plr_ent);
        sts.sts = static_cast<uint8_t>(status);
        // The UUID string identity is seeded by the Replica authority (Mongo `players`),
        // never reconstructed from the FNV hash - migration moves compute, not authority.
        log::info("[PlayerMigDes] player instantiated from delivery: player_ref={} epoch={} chunk=({},{}) dst_region={} ({})",
                  player_ref, snap_epoch, pos.chunk_x, pos.chunk_z, snap.dst_region,
                  plr_found ? "updated in place" : "fresh entity");

        registry.emplace<hub::HubPlrMigFinTag>(dlv_ent);
    }

    /**
     * PASS B - Quittungs-Zustellungen: die Quellkopie darf fallen. Die In-Flight-Pruefung hat
     * der Absender (ase-world) an SEINEM Bestand gemacht; hier steht nur noch die Aufloesung
     * ref → eigene Figur → eigener Kennungstext → eigene Abmeldebitte.
     */
    for (auto [dlv_ent, ack] : ack_view.each()) {
        auto ack_hit = plr_by_ref.find(ack.player_ref);
        if (ack_hit == plr_by_ref.end() || !registry.valid(ack_hit->second)) {
            // NOT_FOUND: die Quittung nennt eine Figur, die dieses Tier nicht (mehr) fuehrt
            log::error(log::ERR::CAT::INVALID_ENTITY, "PlayerMigDesSystem",
                       ack.player_ref, "MIG_ACK_PLAYER_REF");
            registry.emplace<hub::HubPlrMigFinTag>(dlv_ent);
            continue;
        }
        const auto* src_id = registry.try_get<PlayerStaIdntComponent>(ack_hit->second);
        if (src_id == nullptr) {
            // Eine Figur ohne Kennungstext ist selbst zugewandert; ihre Abmeldung gehoert dem
            // Tier, das den Text fuehrt - hier gibt es nichts abzumelden.
            log::error(log::ERR::CAT::COMPONENT_MISSING, "PlayerMigDesSystem",
                       ack.player_ref, "MIG_ACK_IDNT_ROW");
            registry.emplace<hub::HubPlrMigFinTag>(dlv_ent);
            continue;
        }
        auto desp_ent = registry.create();
        auto& desp = registry.emplace<PlayerReqDespComponent>(desp_ent);
        std::memcpy(desp.player_id, src_id->player_id, sizeof(desp.player_id));
        log::info("[PlayerMigDes] migrate ack for player_ref={} epoch={} - source copy despawn requested",
                  ack.player_ref, ack.ack_epoch);
        registry.emplace<hub::HubPlrMigFinTag>(dlv_ent);
    }

    /**
     * PASS C - Verbrauch. Laeuft ueber die Quittung und nimmt die Zeilen + die anstehende
     * Marke ab - alles Typen AUSSERHALB dieser Signatur. remove ist tolerant: eine
     * Snap-Zustellung traegt keine Ack-Zeile und umgekehrt. Die Quittung selbst bleibt auf der
     * leeren Huelle stehen; zerstoert wird sie von ihrem Erzeuger ase-world per Batch.
     */
    for (auto dlv_ent : registry.view<hub::HubPlrMigFinTag>()) {
        registry.remove<hub::HubPlrMigSnapComponent>(dlv_ent);
        registry.remove<hub::HubPlrMigAckComponent>(dlv_ent);
        registry.remove<hub::HubPlrMigPndTag>(dlv_ent);
    }
}

void PlayerMigDesSystem::on_stop(ecs::Registry& registry) {
    log::debug("[PlayerMigDesSystem] Stopped");
    (void)registry;
}

}  // namespace ase::player
