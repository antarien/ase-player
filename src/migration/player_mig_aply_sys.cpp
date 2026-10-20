/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_mig_aply_sys.cpp
 * @brief       PlayerMigAplySystem - build migrated-in figures and honour migrate echoes
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    entity/actor/player
 * @schedule    Reception
 * @design      DSGN_021
 * @usecase     usecase:100692 - migrate figure between regions
 * @actor       Fleet Capacity Ledger (FLTCAP)
 * @created     2026-10-14
 * @modified    2026-10-14
 * @version     1.0.0
 *
 * CAUSAL CHAIN (bridge entities → instantiated figures and released source copies)
 *
 *   [PlayerMigDesSystem copied the inbound rows into PlayerInpMigDlvrComponent /
 *    PlayerInpMigAckComponent bridge entities, the same tick]
 *          │
 *          ▼
 *   ┌───────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerMigAplySystem (Calc)                  │
 *   │                                                           │
 *   │  READS:                                                   │
 *   │    - PlayerInpMigDlvrComponent (snap, snap_len, region)   │
 *   │    - PlayerInpMigAckComponent (player_ref, ack_epoch)     │
 *   │    - PlayerStaEpchComponent (O(1) ref lookup, pre-pass)   │
 *   │    - PlayerStaIdntComponent (despawn identity, OWN row)   │
 *   │                                                           │
 *   │  WRITES:                                                  │
 *   │    - PlayerStaEpch/Pos/Yaw/Vel/Sts/Chk + PlayerSpndTag    │
 *   │      (instantiate or in-place update, OWN types)          │
 *   │    - PlayerReqDespComponent (module-own despawn request)  │
 *   │    - destroys the bridge entities after the walks         │
 *   └───────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   [PlayerHubSpndSystem adopts the new figure as the hub row HUB_PLR_SPND;
 *    PlayerLifeSpwnSystem releases the source copy on the despawn request]
 *
 * WHO SERIALIZES, UNPACKS. PlayerMigSerSystem writes the frozen 42-byte image, this system reads
 * it. ase-world only delivers and touches no player row.
 *
 * THE CHUNK ROW IS CREATED, NOT FILLED. PlayerStaChnkSystem (Dynamics) derives it from the
 * position row and publishes PLR_CHK_X/Z on the star - a second chunk formula here would be a
 * second place for the same derivation.
 *
 * EVERY BRIDGE FALLS. A refused delivery (wrong length, wrong schema, duplicate) is destroyed like
 * an applied one: half an image cannot be told from a whole one, and a bridge that stayed would
 * be applied again on the next tick.
 *
 * HUB Pattern (ARCH_ASE_HUB_API v2.0)
 *
 * READS (from Hub): (none) - the rows arrive through PlayerMigDesSystem
 * WRITES (to Hub): (none) - PlayerHubSpndSystem publishes the figure
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
#include <ase/player/systems/migration/player_mig_aply_sys.hpp>
// Frozen PlayerSnap wire layout (offsets, schema version, size) - L0 contract SSOT
#include <ase/types/region_wire.hpp>
// The bridge components PlayerMigDesSystem wrote
#include <ase/player/components/input/player_inp_mig_dlvr_comp.hpp>
#include <ase/player/components/input/player_inp_mig_ack_comp.hpp>
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

#include <entt/core/hashed_string.hpp>

namespace ase::player {
using namespace entt::literals;

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO View/Query in helpers! (pure binary deserialization only)
 */
namespace {

// Decode one frozen PlayerSnap (little-endian) from a bridge buffer. The caller guarantees the
// buffer holds types::PLAYER_SNAP_SZ bytes. Mirror of encode_player_snap in
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

void PlayerMigAplySystem::on_start(ecs::Registry& registry) {
    log::debug("[PlayerMigAplySystem] Started");
    (void)registry;
}

void PlayerMigAplySystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * EARLY EXIT - no bridge, no index. The pre-pass below costs one walk over all figures; it
     * pays only when there is something to apply.
     */
    auto dlvr_view = registry.view<PlayerInpMigDlvrComponent>();
    auto ack_view = registry.view<PlayerInpMigAckComponent>();
    if (dlvr_view.empty() && ack_view.empty()) {
        return;
    }

    /**
     * PRE-PASS - O(1) index player_ref → figure, once per tick for all bridges of this tick.
     */
    containers::HashMap<uint32_t, ecs::Entity> plr_by_ref;
    for (auto [plr_ent, epch] : registry.view<PlayerStaEpchComponent>().each()) {
        if (epch.player_ref != 0u) {
            plr_by_ref[epch.player_ref] = plr_ent;
        }
    }

    /**
     * PASS A - deliveries: epoch-idempotent instantiate or in-place update. A refused delivery
     * is reported; its bridge falls with the others after the walks.
     */
    for (auto [dlv_ent, dlvr] : dlvr_view.each()) {
        if (dlvr.snap_len != types::PLAYER_SNAP_SZ) {
            // The value exists but is unusable - reported before the bridge falls
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerMigAplySystem",
                      static_cast<uint32_t>(dlv_ent), "snap_len",
                      static_cast<float>(dlvr.snap_len));
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
        decode_player_snap(dlvr.snap, schema, player_ref, snap_epoch,
                           x, y, z, yaw, vx, vy, vz, status);

        if (schema != types::PLAYER_SNAP_SCHEMA_VER) {
            log::warn(log::WRN::CAT::VALUE_INVALID, "PlayerMigAplySystem",
                      static_cast<uint32_t>(dlv_ent), "snap_schema",
                      static_cast<float>(schema));
            continue;
        }

        // Epoch idempotency: a held epoch >= the delivered one is a duplicate or a late migrate -
        // the figure is never simulated twice.
        ecs::Entity plr_ent = ecs::Entity{};
        bool plr_found = false;
        auto ref_hit = plr_by_ref.find(player_ref);
        if (ref_hit != plr_by_ref.end() && registry.valid(ref_hit->second)) {
            plr_ent = ref_hit->second;
            plr_found = true;
            const auto& held = registry.get<PlayerStaEpchComponent>(plr_ent);
            if (held.player_epoch >= snap_epoch) {
                log::info("[PlayerMigAply] duplicate/late migrate for player_ref={} (epoch {} <= held {}) - dropped",
                          player_ref, snap_epoch, held.player_epoch);
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
            // Created, NOT filled: PlayerStaChnkSystem derives the chunk row from the position
            // row and is the ONE place of that derivation.
            registry.emplace<PlayerStaChkComponent>(plr_ent);
            // The embodiment, the same marker as at spawn. PlayerHubSpndSystem carries it into the
            // star as the row HUB_PLR_SPND, over which camera, bdi, geoid and world enumerate the
            // figures; a migrated figure without the marker would be invisible to all of them.
            registry.emplace<PlayerSpndTag>(plr_ent);
            // The new figure goes into the tick index, so a second delivery of the same player in
            // the SAME tick runs as an update instead of creating a double entity.
            plr_by_ref[player_ref] = plr_ent;
        }

        auto& epch = registry.get<PlayerStaEpchComponent>(plr_ent);
        epch.player_ref = player_ref;
        epch.player_epoch = snap_epoch;
        /**
         * CHUNK-RELATIVE: the frozen PlayerSnap carries world metres (f32); the figure keeps the
         * exact chunk address plus local metres - floor and remainder as at spawn
         * (player_life_spwn_sys.cpp), with this module's OWN chunk constant.
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
        // The UUID string identity is seeded by the Replica authority (Mongo `players`), never
        // reconstructed from the FNV hash - migration moves compute, not authority.
        log::info("[PlayerMigAply] player instantiated from delivery: player_ref={} epoch={} chunk=({},{}) dst_region={} ({})",
                  player_ref, snap_epoch, pos.chunk_x, pos.chunk_z, dlvr.dst_region,
                  plr_found ? "updated in place" : "fresh entity");
    }

    /**
     * PASS B - echoes: the source copy may go. The in-flight check was made by the sender
     * (ase-world) against ITS stock; here stands only the resolution ref → own figure → own
     * identity text → own despawn request.
     */
    for (auto [ack_ent, ack] : ack_view.each()) {
        (void)ack_ent;
        auto ack_hit = plr_by_ref.find(ack.player_ref);
        if (ack_hit == plr_by_ref.end() || !registry.valid(ack_hit->second)) {
            // NOT_FOUND: the echo names a figure this tier does not (or no longer) run
            log::error(log::ERR::CAT::INVALID_ENTITY, "PlayerMigAplySystem",
                       ack.player_ref, "MIG_ACK_PLAYER_REF");
            continue;
        }
        const auto* src_id = registry.try_get<PlayerStaIdntComponent>(ack_hit->second);
        if (src_id == nullptr) {
            // A figure without an identity text migrated in itself; its despawn belongs to the
            // tier that holds the text - here there is nothing to despawn.
            log::error(log::ERR::CAT::COMPONENT_MISSING, "PlayerMigAplySystem",
                       ack.player_ref, "MIG_ACK_IDNT_ROW");
            continue;
        }
        auto desp_ent = registry.create();
        auto& desp = registry.emplace<PlayerReqDespComponent>(desp_ent);
        std::memcpy(desp.player_id, src_id->player_id, sizeof(desp.player_id));
        log::info("[PlayerMigAply] migrate ack for player_ref={} epoch={} - source copy despawn requested",
                  ack.player_ref, ack.ack_epoch);
    }

    /**
     * BRIDGES FALL - after the walks, as one batch outside every iteration
     * (WRFL_ASE_DEFERRED_DELETION). This system is their only reader.
     */
    registry.destroy(dlvr_view.begin(), dlvr_view.end());
    registry.destroy(ack_view.begin(), ack_view.end());
}

void PlayerMigAplySystem::on_stop(ecs::Registry& registry) {
    log::debug("[PlayerMigAplySystem] Stopped");
    (void)registry;
}

}  // namespace ase::player
