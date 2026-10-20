/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_mig_des_sys.cpp
 * @brief       PlayerMigDesSystem - take the inbound migrate rows off the star into bridge components
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    entity/actor/player
 * @schedule    Reception
 * @design      DSGN_021
 * @usecase     usecase:100692 - migrate figure between regions
 * @actor       Fleet Capacity Ledger (FLTCAP)
 * @created     2026-08-19
 * @modified    2026-10-14
 * @version     2.0.0
 *
 * CAUSAL CHAIN (Hub rows → bridge entities of this module)
 *
 *   [WorldPlrMigRcvSystem of ase-world filed rows: HUB_PLR_MIG_DLV (a frozen PlayerSnap to
 *    unpack) or HUB_PLR_MIG_ACK (an echo for a figure that left)]
 *          │
 *          ▼
 *   ┌───────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerMigDesSystem (Sync, server only)      │
 *   │                                                           │
 *   │  READS:                                                   │
 *   │    - hub row HUB_PLR_MIG_DLV (SNAP, DST_REGION)           │
 *   │    - hub row HUB_PLR_MIG_ACK (PLAYER_REF, EPOCH)          │
 *   │                                                           │
 *   │  WRITES:                                                  │
 *   │    - PlayerInpMigDlvrComponent on a new bridge entity     │
 *   │    - PlayerInpMigAckComponent on a new bridge entity      │
 *   │    - row_release of every row it visited                  │
 *   └───────────────────────────────────────────────────────────┘
 *          │
 *          │ the same tick, run_after
 *          ▼
 *   [PlayerMigAplySystem builds or updates the figure, files the despawn request and destroys
 *    the bridge entities]
 *
 * SYN CUT. This system only copies: the rows of the star into components of this module, and the
 * rows are released in the same visit (row_each allows releasing the visited row). Checking the
 * image, the epoch comparison, the chunk split and the despawn request are calculation and live in
 * PlayerMigAplySystem, which never touches the hub.
 *
 * HUB Pattern (ARCH_ASE_HUB_API v2.0)
 *
 * READS (from Hub): row HUB_PLR_MIG_DLV (HUB_PLR_MIG_DLV_SNAP, HUB_PLR_MIG_DLV_DST_REGION),
 *                   row HUB_PLR_MIG_ACK (HUB_PLR_MIG_ACK_PLAYER_REF, HUB_PLR_MIG_ACK_EPOCH)
 * WRITES (to Hub): row_release of the visited rows
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
// Frozen PlayerSnap size - L0 contract SSOT
#include <ase/types/region_wire.hpp>
// The inbound rows of the migrate seam - via the ONE allowed hub entry
#include <ase/hub/api.hpp>
// The bridge components the calc system reads
#include <ase/player/components/input/player_inp_mig_dlvr_comp.hpp>
#include <ase/player/components/input/player_inp_mig_ack_comp.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

#include <entt/core/hashed_string.hpp>

namespace ase::player {
using namespace entt::literals;

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * This system only copies rows into components and needs no helper.
 */
namespace {

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerMigDesSystem::on_start(ecs::Registry& registry) {
    log::debug("[PlayerMigDesSystem] Started");
    (void)registry;
}

void PlayerMigDesSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * DELIVERIES - one bridge entity per HUB_PLR_MIG_DLV row. The image is copied as it stands,
     * at most the contract size; the stored length goes along so the calc system can refuse a
     * short or long image. The row is released in the same visit.
     */
    hub::row_each(registry, "HUB_PLR_MIG_DLV"_hs, [&registry](ecs::Entity row) {
        const uint32_t owner = static_cast<uint32_t>(row);
        const auto bridge = registry.create();
        auto& inp = registry.emplace<PlayerInpMigDlvrComponent>(bridge);
        inp.snap_len = hub::bytes_len(registry, owner, "HUB_PLR_MIG_DLV_SNAP"_hs);
        (void)hub::get_bytes(registry, owner, "HUB_PLR_MIG_DLV_SNAP"_hs, inp.snap,
                             types::PLAYER_SNAP_SZ);
        if (!hub::try_get_u32(registry, owner, "HUB_PLR_MIG_DLV_DST_REGION"_hs, inp.dst_region)) {
            log::error(log::ERR::CAT::HUB_NOT_FOUND, "PlayerMigDesSystem", owner,
                       "HUB_PLR_MIG_DLV_DST_REGION");
        }
        hub::row_release(registry, row);
    });

    /**
     * ECHOES - one bridge entity per HUB_PLR_MIG_ACK row; a row without its identity is reported
     * and released without a bridge, because it names no figure.
     */
    hub::row_each(registry, "HUB_PLR_MIG_ACK"_hs, [&registry](ecs::Entity row) {
        const uint32_t owner = static_cast<uint32_t>(row);
        uint32_t player_ref = 0u;
        uint32_t ack_epoch = 0u;
        if (!hub::try_get_u32(registry, owner, "HUB_PLR_MIG_ACK_PLAYER_REF"_hs, player_ref) ||
            !hub::try_get_u32(registry, owner, "HUB_PLR_MIG_ACK_EPOCH"_hs, ack_epoch)) {
            log::error(log::ERR::CAT::HUB_NOT_FOUND, "PlayerMigDesSystem", owner,
                       "HUB_PLR_MIG_ACK_FIELDS");
            hub::row_release(registry, row);
            return;
        }
        const auto bridge = registry.create();
        auto& inp = registry.emplace<PlayerInpMigAckComponent>(bridge);
        inp.player_ref = player_ref;
        inp.ack_epoch = ack_epoch;
        hub::row_release(registry, row);
    });
}

void PlayerMigDesSystem::on_stop(ecs::Registry& registry) {
    log::debug("[PlayerMigDesSystem] Stopped");
    (void)registry;
}

}  // namespace ase::player
