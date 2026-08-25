/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_acc_mov_sys.cpp
 * @brief       PlayerAccMovSystem - Engine-level anti-cheat movement detector
 * @description SERVER-ONLY authority: flags a player whose realised horizontal speed exceeds the
 *              engine movement authority as the contract trigger "PLAYER_MOVEMENT_SUSPICIOUS".
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state/abundance/change
 * @schedule    Dynamics
 * @created     2026-06-30
 * @modified    2026-06-30
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_ACC_MOV: realised velocity → anti-cheat flag)
 *
 *   [PlayerStaVelComponent — realised velocity after PlayerSimPhysSystem]
 *          │
 *          │ vx, vz (horizontal)
 *          ▼
 *   ┌──────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerAccMovSystem (SERVER-ONLY)            │
 *   │                                                          │
 *   │  READS:                                                  │
 *   │    → PlayerStaIdntComponent (player identity)               │
 *   │    → PlayerStaVelComponent (realised velocity)            │
 *   │                                                          │
 *   │  WRITES (Hub):                                           │
 *   │    → "PLAYER_MOVEMENT_SUSPICIOUS"_hs (1.0 / 0.0)         │
 *   └──────────────────────────────────────────────────────────┘
 *          │
 *          │ flag broadcast World → Replica (Hub dirty-tracking, transitions only)
 *          ▼
 *   Replica forwards as BIN_MSG_RSN_TRIGGER_BATCH(18) → MovementValidator (Reasoning)
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1)):
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub for the Replica forwarder):
 *   "PLAYER_MOVEMENT_SUSPICIOUS"_hs → 1.0 when realised horizontal speed^2 exceeds the authority, else 0.0
 *
 * NOTE: SERVER-ONLY authority — never transpiled to the client (a cheating client must not run the
 * detector). The squared comparison avoids math::sqrt and is frequency-independent (velocity is m/s).
 * Hub dirty-tracking broadcasts only on transition, so the flag re-arms and never spams.
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
#include <ase/player/systems/anticheat/player_acc_mov_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_acmp_comp.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// Hub for O(1) API
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

namespace ase::player {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

// No helper functions needed - the authority check is a single squared comparison.

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerAccMovSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerAccMovSystem] Started");
}

void PlayerAccMovSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * Per player: compare the realised horizontal speed (squared) against the engine movement
     * authority. PlayerCtrlMovSystem caps legit velocity at run_speed, so a realised horizontal
     * speed above run_speed * margin cannot come from legitimate input — it is a speed-hack.
     * Flag it on the Hub; the Replica forwards it as the MovementValidator contract trigger.
     */
    auto view = registry.view<PlayerStaIdntComponent, PlayerStaVelComponent, PlayerStaPosComponent>();

    for (auto [entity, id, vel, pos] : view.each()) {
        (void)id;  // identity component selects player entities; fields unused here
        uint32_t owner = static_cast<uint32_t>(entity);

        // Speed-hack authority: realised horizontal speed (squared) above the run_speed*margin bound.
        const float horizontal_speed_sq = vel.vx * vel.vx + vel.vz * vel.vz;
        bool suspicious = horizontal_speed_sq > PLR_AC_MAX_HORIZONTAL_SPEED_SQ;

        // Teleport authority: a per-tick horizontal position jump beyond PLR_AC_TELEPORT_STEP cannot be
        // legitimate single-tick motion. The PlayerStaAcmpComponent PRESENCE is the baseline-valid signal
        // (no boolean field) — the first observed tick records the baseline and never false-positives.
        auto* acmp = registry.try_get<PlayerStaAcmpComponent>(entity);
        if (acmp != nullptr) {
            /**
             * Delta chunk-relativ (S2b 2026-08-11): Wabendifferenz exakt in int, Restmeter in
             * kleinen floats - die fruehere Differenz zweier absoluter f32-Weltmeter war bei
             * grossen Adressen auf die ULP (bis 16 m) gerastert und damit als Teleport-Mass
             * unbrauchbar.
             */
            const float dx = static_cast<float>(pos.chunk_x - acmp->last_chunk_x) *
                                 MOVEMENT_DEFAULT_CHUNK_SIZE +
                             (pos.local_x - acmp->last_local_x);
            const float dz = static_cast<float>(pos.chunk_z - acmp->last_chunk_z) *
                                 MOVEMENT_DEFAULT_CHUNK_SIZE +
                             (pos.local_z - acmp->last_local_z);
            if (dx * dx + dz * dz > PLR_AC_TELEPORT_STEP_SQ) {
                suspicious = true;
            }
            acmp->last_chunk_x = pos.chunk_x;
            acmp->last_chunk_z = pos.chunk_z;
            acmp->last_local_x = pos.local_x;
            acmp->last_local_z = pos.local_z;
        } else {
            auto& baseline = registry.emplace<PlayerStaAcmpComponent>(entity);
            baseline.last_chunk_x = pos.chunk_x;
            baseline.last_chunk_z = pos.chunk_z;
            baseline.last_local_x = pos.local_x;
            baseline.last_local_z = pos.local_z;
        }

        // The detector MUST run in Dynamics (30Hz) — the teleport check above compares the per-tick
        // position delta, so it cannot move to a slower schedule. hub::set is change-based (it only
        // broadcasts on a value change), so publishing every tick does NOT spam the wire. The warn,
        // however, would spam the World log 30x/s while a cheat is sustained — so log ONLY on the rising
        // edge (was-not-suspicious → suspicious): the flag ONSET is the event, not each tick it persists.
        const bool was_suspicious =
            hub::exists(registry, owner, "PLAYER_MOVEMENT_SUSPICIOUS"_hs) &&
            hub::get(registry, owner, "PLAYER_MOVEMENT_SUSPICIOUS"_hs) >= 0.5f;
        hub::set(registry, owner, "PLAYER_MOVEMENT_SUSPICIOUS"_hs, suspicious ? 1.0f : 0.0f);

        if (suspicious && !was_suspicious) {
            log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "PlayerAccMovSystem", owner,
                      "horizontal_speed_sq", horizontal_speed_sq, 0.0f,
                      PLR_AC_MAX_HORIZONTAL_SPEED_SQ);
        }
    }
}

void PlayerAccMovSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerAccMovSystem] Stopped");
}

}  // namespace ase::player
