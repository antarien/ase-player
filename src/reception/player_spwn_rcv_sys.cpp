/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_spwn_rcv_sys.cpp
 * @brief       PlayerSpwnRcvSystem - creates a spawn request from the Replica-forwarded backend spawn frame
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    action/transfer/receive
 * @schedule    Reception
 * @created     2026-07-01
 * @modified    2026-07-01
 * @version     1.0.0
 *
 * CAUSAL CHAIN (PLAYER_SPWN_RCV: backend-driven spawn, no human client)
 *
 *   [ase-cli player spawn → BIN_MSG_PLAYER_SPAWN → Replica → World publisher]
 *          │
 *          │ World kernel demux (KernelWbskDspcSystem) routes byte[0]==77 → LANE_SPW
 *          ▼
 *   ┌───────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerSpwnRcvSystem                             │
 *   │                                                               │
 *   │  READS:  transport::InboundQueueResourceManager (pop LANE_SPW)│
 *   │                                                               │
 *   │  WRITES: PlayerReqSpwnComponent (one request entity created) │
 *   └───────────────────────────────────────────────────────────────┘
 *          │
 *          │ PlayerLifeSpwnSystem consumes PlayerReqSpwnComponent
 *          ▼
 *   [real ase-player entity spawned (PlayerStId/Pos/Vel/Phys + PlayerSpndTag)]
 *
 * HUB Pattern (transport lane, websocket-FREE)
 *
 * READS (from ctx):
 *   transport::InboundQueueResourceManager: pop LANE_SPW frames
 *
 * WRITES (to ECS):
 *   PlayerReqSpwnComponent: player_id + x + z (one request entity per frame)
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
#include <ase/player/systems/reception/player_spwn_rcv_sys.hpp>
// Transport inbound lane (L1 via ctx — the L2 demux fills LANE_SPW; websocket-FREE)
#include <ase/transport/inbound_queue_resource_manager.hpp>
#include <ase/transport/types.hpp>
// Module constants (the local BIN_MSG mirror + frame offsets/bounds)
#include <ase/player/types.hpp>
// Spawn request component (same module — PlayerLifeSpwnSystem consumes it)
#include <ase/player/components/request/player_req_spwn_comp.hpp>
// The errand companion of a spawn request - present only when the frame asked for a walker
#include <ase/player/components/request/player_req_roam_comp.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>

namespace ase::player {
using namespace entt::literals;

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO View/Query in helpers; pure binary deserialization only.
 */
namespace {

// No helper needed — the spawn frame is a fixed-offset POD decode done inline under the bounds check.

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerSpwnRcvSystem::on_start(ecs::Registry& registry) {
    log::debug("[PlayerSpwnRcvSystem] Started");

    // Register the forwarded spawn wire-type onto the shared inbound lane so the L2 demux routes it here.
    // Emplace the queue if the demux on_start has not run yet (order-independent), mirroring the World
    // reception drainers.
    if (registry.ctx().find<transport::InboundQueueResourceManager>() == nullptr) {
        registry.ctx().emplace<transport::InboundQueueResourceManager>();
    }
    auto& queue = registry.ctx().get<transport::InboundQueueResourceManager>();
    queue.register_route(PLR_BIN_MSG_PLAYER_SPAWN, transport::LANE_SPW);
}

void PlayerSpwnRcvSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* queue = registry.ctx().find<transport::InboundQueueResourceManager>();
    if (queue == nullptr || !queue->has_inbound(transport::LANE_SPW)) return;

    char buf[transport::LANE_BUF_SZ] = {};
    uint32_t msg_len = 0;
    uint32_t drained = 0;

    while (drained < PLR_SPW_RCV_BATCH_MAX &&
           queue->pop_inbound(transport::LANE_SPW, buf, transport::LANE_BUF_SZ, msg_len)) {
        ++drained;
        if (msg_len < PLR_SPW_FRAME_SZ) {
            msg_len = 0;
            continue;
        }
        if (static_cast<uint8_t>(buf[0]) != PLR_BIN_MSG_PLAYER_SPAWN) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "PlayerSpwnRcvSystem", "inbound_type",
                      static_cast<float>(static_cast<uint8_t>(buf[0])));
            msg_len = 0;
            continue;
        }

        // Create the spawn request the same shape the REST/integration path documents: player_id[64] + x + z.
        // PlayerLifeSpwnSystem (same module, Dynamics) consumes it next tick and creates the real entity.
        auto request_entity = registry.create();
        auto& request = registry.emplace<PlayerReqSpwnComponent>(request_entity);
        std::memcpy(request.player_id, buf + PLR_SPW_PLAYER_ID_OFF, PLR_SPW_PLAYER_ID_LEN);
        request.player_id[PLR_SPW_PLAYER_ID_LEN - 1u] = '\0';
        std::memcpy(&request.x, buf + PLR_SPW_X_OFF, 4);
        std::memcpy(&request.z, buf + PLR_SPW_Z_OFF, 4);

        // THE ERRAND IS A COMPANION, NOT A FIELD. A frame that asks for a walker gets a second
        // component on the SAME request entity; a frame that asks for a plain spawn gets none, and
        // the spawn stays exactly what it always was. That is what keeps "no speed given" apart
        // from "speed given as zero" without either of them needing a sentinel.
        float roam_speed = PLR_SPW_SPEED_NONE;
        std::memcpy(&roam_speed, buf + PLR_SPW_SPEED_OFF, 4);
        if (roam_speed > PLR_SPW_SPEED_NONE) {
            auto& errand = registry.emplace<PlayerReqRoamComponent>(request_entity);
            errand.speed = roam_speed;
        }

        log::info("[PlayerSpwnRcv] backend spawn request created (player_id={} x={} z={} speed={})",
                  request.player_id, request.x, request.z, roam_speed);
        msg_len = 0;
    }
}

void PlayerSpwnRcvSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerSpwnRcvSystem] Stopped");
}

}  // namespace ase::player
