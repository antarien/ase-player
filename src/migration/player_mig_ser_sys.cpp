/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_mig_ser_sys.cpp
 * @brief       PlayerMigSerSystem - serialize the PlayerSnap for an armed boundary migrate
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    entity/actor/player
 * @schedule    Observation
 * @created     2026-07-28
 * @modified    2026-08-14
 * @version     1.0.0
 *
 * CAUSAL CHAIN (PLR_MIG_SER: armed migrate request → staged PlayerSnap bytes)
 *
 *   [WorldPlrBndSystem armed PlayerReqMigComponent on a player whose origin chunk
 *    dwelt past the boundary hysteresis (WS-H.4)]
 *          │
 *          │ the player's live state must cross the wire in the frozen layout
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerMigSerSystem (World tier, Observation)  │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    - PlayerReqMigComponent (the armed migrate)              │
 *   │    - PlayerStaPosComponent (x, y, z, yaw)                    │
 *   │    - PlayerStaVelComponent (vx, vy, vz)                      │
 *   │    - PlayerStaStsComponent (status, widened u8→u32)          │
 *   │    - PlayerStaIdntComponent (UUID string → FNV-1a32 wire id)   │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    - PlayerStaEpchComponent (seed player_ref, bump epoch)   │
 *   │    - PlayerMigStgPndTag (watch pass, hand-over marker)      │
 *   │    - PlayerBufMigComponent (stage pass, after the walk -    │
 *   │      the watch pass excludes this very type)                │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ exactly one staged image per armed request (buffer excluded from re-entry)
 *          ▼
 *   [WorldPlrMigSndSystem wraps the buffer into [100][epoch][proj][dst] and ships it]
 *
 *   TWO PASSES, and the exclude did NOT move: the staged-delivery exclusion is the re-entry
 *   barrier ACROSS ticks (it is what makes the epoch bump exactly once per armed migrate), the
 *   marker is the hand-over WITHIN one tick. Merging them would let the next tick re-enter the
 *   walk and bump the epoch a second time - the send-side case in test_world_rgn.cpp proves the
 *   consumed delivery, and re-entry is barred by exclude<hub::HubPlrMigSnapComponent>.
 *
 *   LOG ORDER, measured 2026-08-14: the stage pass walks the marker pool, and EnTT iterates a pool
 *   BACKWARDS (storage.hpp:553 - begin() starts at size() and counts down). With several players
 *   migrating in the same tick the "PlayerSnap staged" lines therefore appear in a different order
 *   than before. No order was ever guaranteed: the watch pass runs over six pools and EnTT drives
 *   the SMALLEST one, which shifts with the population. Line text, line count and every value are
 *   unchanged - only the sequence among simultaneous migrates. Noted here so nobody diffs two logs
 *   and hunts a ghost.
 *
 * HUB Pattern (ARCH_ASE_HUB_API v2.0)
 *
 * READS (from Hub): (none)
 * WRITES (to Hub): (none) - the snap rides the transport lane, not the Hub.
 *
 * FLYWEIGHT PATTERN (Not used - the 42-byte image is a fixed inline attribute)
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
 * [ ] Safe deletion (first collect, then delete)?
 * [ ] Not deleting other entities during iteration?
 * [ ] Not invalidating references during iteration?
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
 * [ ] NO ARRAYS! (use Entity-per-Item + Tags!)
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
 * [ ] NO switch/case statements? (use Tag-filtered Views or lookup tables!)
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
#include <ase/player/systems/migration/player_mig_ser_sys.hpp>
// Frozen PlayerSnap wire layout (offsets, schema version) - L0 contract SSOT
#include <ase/types/region_wire.hpp>
// Die Anweisung der Grenzwache und die gestagte Zustellung laufen seit 2026-08-19 ueber den
// Stern (HubPlrMigArmComponent / HubPlrMigSnapComponent auf der FIGUR): ase-world musste fuer
// die alte player::PlayerReqMigComponent einen fremden Typ SCHREIBEN und fuer den alten
// PlayerBufMigComponent einen fremden Typ LESEN - beide Kanten sind damit gefallen.
#include <ase/hub/api.hpp>
// Components: the live state sources + epoch
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_yaw_comp.hpp>
// types.hpp for constants (chunk edge for the world-metre wire sum)
#include <ase/player/types.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_sts_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/state/player_sta_epch_comp.hpp>
// Transient marker of the watch pass: PlayerBufMigComponent sits in that walk's entt::exclude, so
// the buffer is staged by the pass after the walk. The exclude itself is untouched - it is the
// re-entry barrier across ticks, this marker is the hand-over within one.
#include <ase/player/components/tag/player_mig_stg_pnd_tag.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>

#include <entt/core/hashed_string.hpp>

namespace ase::player {
using namespace entt::literals;

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO View/Query in helpers! (pure binary serialization only)
 */
namespace {

// Encode the frozen 42-byte PlayerSnap (little-endian) into a caller buffer of
// >= types::PLAYER_SNAP_SZ:
//   [schema_ver:u16][player_id:u32][player_epoch:u32]
//   [x:f32][y:f32][z:f32][yaw:f32][vx:f32][vy:f32][vz:f32][status:u32]
// status is the live u8 player state widened to the u32 wire field. Returns the byte count
// (types::PLAYER_SNAP_SZ). Pure: memcpy only, no View/Query/Registry.
uint32_t encode_player_snap(char* out, uint32_t player_ref, uint32_t player_epoch,
                            float x, float y, float z, float yaw,
                            float vx, float vy, float vz, uint8_t sts) {
    uint16_t schema = types::PLAYER_SNAP_SCHEMA_VER;
    std::memcpy(out + types::PLAYER_SNAP_OFF_SCHEMA, &schema, 2);
    std::memcpy(out + types::PLAYER_SNAP_OFF_ID, &player_ref, 4);
    std::memcpy(out + types::PLAYER_SNAP_OFF_EPOCH, &player_epoch, 4);
    std::memcpy(out + types::PLAYER_SNAP_OFF_POS, &x, 4);
    std::memcpy(out + types::PLAYER_SNAP_OFF_POS + 4u, &y, 4);
    std::memcpy(out + types::PLAYER_SNAP_OFF_POS + 8u, &z, 4);
    std::memcpy(out + types::PLAYER_SNAP_OFF_POS + 12u, &yaw, 4);
    std::memcpy(out + types::PLAYER_SNAP_OFF_VEL, &vx, 4);
    std::memcpy(out + types::PLAYER_SNAP_OFF_VEL + 4u, &vy, 4);
    std::memcpy(out + types::PLAYER_SNAP_OFF_VEL + 8u, &vz, 4);
    const uint32_t status = static_cast<uint32_t>(sts);
    std::memcpy(out + types::PLAYER_SNAP_OFF_STATUS, &status, 4);
    return types::PLAYER_SNAP_SZ;
}

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerMigSerSystem::on_start(ecs::Registry& registry) {
    log::debug("[PlayerMigSerSystem] Started");
    (void)registry;
}

void PlayerMigSerSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * SERIALIZE PASS
     * One staged image per armed request: the buffer exclusion makes re-entry impossible, so
     * the epoch bumps exactly once per migrate. player_ref seeds lazily from the UUID string
     * (FNV-1a32 - the u32 identity the frozen PlayerSnap carries).
     */
    for (auto plr_ent :
         registry.view<hub::HubPlrMigArmComponent, PlayerStaPosComponent, PlayerStaYawComponent,
                       PlayerStaVelComponent, PlayerStaStsComponent, PlayerStaIdntComponent>(
             entt::exclude<hub::HubPlrMigSnapComponent>)) {
        auto& epch = registry.get_or_emplace<PlayerStaEpchComponent>(plr_ent);
        if (epch.player_ref == 0u) {
            epch.player_ref =
                entt::hashed_string::value(registry.get<PlayerStaIdntComponent>(plr_ent).player_id);
        }
        ++epch.player_epoch;
        registry.emplace_or_replace<PlayerMigStgPndTag>(plr_ent);
    }

    /**
     * STAGE PASS
     * Writes the buffer the watch pass armed. PlayerBufMigComponent sits in the walk's
     * entt::exclude, so emplacing it from inside dropped the row out of the very set being
     * iterated. Nothing had to be carried out: every value below still sits on the entity, and
     * snap_len is produced by the encode itself. The epoch was bumped ONCE in the walk above and
     * is only read here.
     *
     * DER DRAHT BLEIBT WELTMETER (eingefrorener PlayerSnap, 4x f32): die Summe
     * chunk * Kante + local ist an Wabenkanten exakt (S2b 2026-08-11); das Wire-v2
     * mit (chunk:i32, local:f32) ist TASKREGISTER:1724, Entscheidungsklasse Betreiber.
     */
    for (auto stg_ent : registry.view<PlayerMigStgPndTag>()) {
        const auto& epch = registry.get<PlayerStaEpchComponent>(stg_ent);
        const auto& arm = registry.get<hub::HubPlrMigArmComponent>(stg_ent);
        const auto& pos = registry.get<PlayerStaPosComponent>(stg_ent);
        const auto& yaw = registry.get<PlayerStaYawComponent>(stg_ent);
        const auto& vel = registry.get<PlayerStaVelComponent>(stg_ent);
        const auto& sts = registry.get<PlayerStaStsComponent>(stg_ent);

        // Gestaged wird die STERN-Zustellung auf der Figur selbst: der Versender in ase-world
        // liest Arm + Snap ueber die Fassade und probt die Epoche aus dem Bild (L0-Offsets) -
        // kein player-Typ mehr auf seiner Seite.
        auto& snap_dlv = registry.emplace<hub::HubPlrMigSnapComponent>(stg_ent);
        snap_dlv.snap_len = encode_player_snap(
            snap_dlv.snap, epch.player_ref, epch.player_epoch,
            static_cast<float>(pos.chunk_x) * MOVEMENT_DEFAULT_CHUNK_SIZE + pos.local_x,
            pos.y,
            static_cast<float>(pos.chunk_z) * MOVEMENT_DEFAULT_CHUNK_SIZE + pos.local_z,
            yaw.yaw, vel.vx, vel.vy, vel.vz, sts.sts);
        snap_dlv.player_ref = epch.player_ref;
        snap_dlv.dst_region = arm.dst_region;
        // Aufzaehlbarkeits-Marke im SELBEN Zug wie die Zeile (HUB_COMPONENT_ITERATION): der
        // Versender zaehlt ueber sie auf, nie ueber die Hub-Zeilen selbst.
        registry.emplace<hub::HubPlrMigSndPndTag>(stg_ent);
        log::info("[PlayerMigSer] PlayerSnap staged: player_ref={} epoch={} dst_region={} proj_hash={} ({} bytes)",
                  epch.player_ref, epch.player_epoch, arm.dst_region, arm.proj_hash,
                  snap_dlv.snap_len);
    }
    registry.clear<PlayerMigStgPndTag>();
}

void PlayerMigSerSystem::on_stop(ecs::Registry& registry) {
    log::debug("[PlayerMigSerSystem] Stopped");
    (void)registry;
}

}  // namespace ase::player
