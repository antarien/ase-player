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
 * @modified    2026-07-28
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
 *   │    - PlayerStPosComponent (x, y, z, yaw)                    │
 *   │    - PlayerStVelComponent (vx, vy, vz)                      │
 *   │    - PlayerStStsComponent (status, widened u8→u32)          │
 *   │    - PlayerStIdComponent (UUID string → FNV-1a32 wire id)   │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    - PlayerStaEpchComponent (seed player_ref, bump epoch)   │
 *   │    - PlayerBufMigComponent (the staged 42-byte PlayerSnap)  │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ exactly one staged image per armed request (buffer excluded from re-entry)
 *          ▼
 *   [WorldPlrMigSndSystem wraps the buffer into [100][epoch][proj][dst] and ships it]
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
// Components: the armed request, the live state sources, epoch + staged buffer
#include <ase/player/components/request/player_req_mig_comp.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_sta_epch_comp.hpp>
#include <ase/player/components/buffer/player_buf_mig_comp.hpp>
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
    for (auto [plr_ent, req, pos, vel, sts, idc] :
         registry.view<PlayerReqMigComponent, PlayerStPosComponent, PlayerStVelComponent,
                       PlayerStStsComponent, PlayerStIdComponent>(
             entt::exclude<PlayerBufMigComponent>).each()) {
        auto& epch = registry.get_or_emplace<PlayerStaEpchComponent>(plr_ent);
        if (epch.player_ref == 0u) {
            epch.player_ref = entt::hashed_string::value(idc.player_id);
        }
        ++epch.player_epoch;

        auto& snap_buf = registry.emplace<PlayerBufMigComponent>(plr_ent);
        snap_buf.snap_len = encode_player_snap(snap_buf.snap, epch.player_ref, epch.player_epoch,
                                               pos.x, pos.y, pos.z, pos.yaw,
                                               vel.vx, vel.vy, vel.vz, sts.sts);
        log::info("[PlayerMigSer] PlayerSnap staged: player_ref={} epoch={} dst_region={} proj_hash={} ({} bytes)",
                  epch.player_ref, epch.player_epoch, req.dst_region, req.proj_hash,
                  snap_buf.snap_len);
    }
}

void PlayerMigSerSystem::on_stop(ecs::Registry& registry) {
    log::debug("[PlayerMigSerSystem] Stopped");
    (void)registry;
}

}  // namespace ase::player
