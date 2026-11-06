/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_cmb_syn_sys.cpp
 * @brief       PlayerCmbSynSystem - lays the movement restriction of the combat star on each player figure
 * @description SYN PATTERN: reads slow, frost, landing stun, blunder stun and knockback of every player
 *              figure from the combat star into PlayerInpCmbComponent and PlayerInpCmbKnbkComponent; the
 *              mover PlayerCtrlMovSystem and the knockback step PlayerSimKnbkSystem read only those rows.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    input
 * @parity      server_only
 * @schedule    Ingestion
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-06
 * @modified    2026-11-06
 * @version     1.0.0
 *
 * CAUSAL CHAIN (combat star → movement restriction of the player figure)
 *
 *   [Hub "CMB_SLW_*", "CMB_FRZ_STA_SLOW_FACTOR", "CMB_LANDING_STUN", "CMB_PATZER_STUN", "CMB_KBK_*"
 *    — published per figure by the hub publishers of ase-combat (owner = entity)]
 *          │
 *          ▼
 *   ┌──────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerCmbSynSystem (SERVER-ONLY)           │
 *   │                                                          │
 *   │  READS:                                                  │
 *   │    → PlayerStaPosComponent, PlayerStaYawComponent (sel.) │
 *   │    → Hub slow, frost, two stuns, knockback (per figure)  │
 *   │                                                          │
 *   │  WRITES:                                                 │
 *   │    → PlayerInpCmbComponent (every figure, every pass)    │
 *   │    → PlayerInpCmbKnbkComponent (only while knocked back) │
 *   └──────────────────────────────────────────────────────────┘
 *          │
 *          ▼
 *   PlayerCtrlMovSystem scales the target speed; PlayerSimKnbkSystem moves the position
 *
 * NOT AFFLICTED IS THE NORMAL STATE. A key the star does not carry for the figure writes 0 without
 * a log line, so a figure outside combat moves at full speed. A slow, frost or flag value outside
 * [0, 1] and a negative stun are reported and write 0. The knockback row stands only while
 * CMB_KBK_ACTIVE is set and all three displacements are found; otherwise it is removed.
 *
 * AXES OF THE KNOCKBACK: CmbStatKbkDispComponent keeps X and Y on the ground and Z as the height.
 * The row carries them unchanged; PlayerSimKnbkSystem maps them onto the player axes.
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1)):
 *
 * READS (from Hub, owner = player figure entity):
 *   "CMB_SLW_ACTIVE"_hs           → slow flag, 0 to 1
 *   "CMB_SLW_INTENSITY"_hs        → slow fraction of the target speed, 0 to 1
 *   "CMB_FRZ_STA_SLOW_FACTOR"_hs  → frost fraction of the target speed, 0 to 1
 *   "CMB_LANDING_STUN"_hs         → seconds of landing stun remaining
 *   "CMB_PATZER_STUN"_hs          → seconds of blunder recovery remaining
 *   "CMB_KBK_ACTIVE"_hs           → knockback flag, 0 to 1
 *   "CMB_KBK_DISP_X/Y/Z"_hs       → knockback displacement in this frame, metres of either sign
 *
 * WRITES (to Hub): none
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
#include <ase/player/systems/sync/player_cmb_syn_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/input/player_inp_cmb_comp.hpp>
#include <ase/player/components/input/player_inp_cmb_knbk_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_yaw_comp.hpp>
// types.hpp for constants
#include <ase/player/types.hpp>
// ase-types (Layer 0) for NOT_FOUND and range predicates
#include <ase/types/types.hpp>
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

// No helper functions: every Hub read stands in tick() with its own NOT_FOUND and range test.

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerCmbSynSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerCmbSynSystem] Started");
}

void PlayerCmbSynSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // The figures PlayerSyncInpSystem gives a movement input; spawned and migrated figures alike.
    for (auto [entity, pos, yaw] : registry.view<PlayerStaPosComponent, PlayerStaYawComponent>().each()) {
        (void)pos;
        (void)yaw;
        const uint32_t owner = static_cast<uint32_t>(entity);
        auto& cmb = registry.get_or_emplace<PlayerInpCmbComponent>(entity);

        // Slow: the intensity counts only while the slow flag is set.
        const float slow_active = hub::get(registry, owner, "CMB_SLW_ACTIVE"_hs);
        if (!ase::types::is_not_found(slow_active) &&
            !ase::types::is_in_rng_f(slow_active, PLR_CMB_UNIT_MIN, PLR_CMB_UNIT_MAX)) {
            log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "PlayerCmbSynSystem", owner, "CMB_SLW_ACTIVE",
                      slow_active, PLR_CMB_UNIT_MIN, PLR_CMB_UNIT_MAX);
        }
        const float slow_intensity = hub::get(registry, owner, "CMB_SLW_INTENSITY"_hs);
        if (!ase::types::is_not_found(slow_intensity) &&
            !ase::types::is_in_rng_f(slow_intensity, PLR_CMB_UNIT_MIN, PLR_CMB_UNIT_MAX)) {
            log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "PlayerCmbSynSystem", owner, "CMB_SLW_INTENSITY",
                      slow_intensity, PLR_CMB_UNIT_MIN, PLR_CMB_UNIT_MAX);
        }
        const bool slow_set = ase::types::is_in_rng_f(slow_active, PLR_CMB_FLAG_SET_MIN, PLR_CMB_UNIT_MAX);
        cmb.slow_intensity =
            (slow_set && ase::types::is_in_rng_f(slow_intensity, PLR_CMB_UNIT_MIN, PLR_CMB_UNIT_MAX))
                ? slow_intensity
                : PLR_CMB_UNIT_MIN;

        // Frost: a movement reduction of its own, multiplied with the slow by the mover.
        const float freeze_slow = hub::get(registry, owner, "CMB_FRZ_STA_SLOW_FACTOR"_hs);
        if (!ase::types::is_not_found(freeze_slow) &&
            !ase::types::is_in_rng_f(freeze_slow, PLR_CMB_UNIT_MIN, PLR_CMB_UNIT_MAX)) {
            log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "PlayerCmbSynSystem", owner, "CMB_FRZ_STA_SLOW_FACTOR",
                      freeze_slow, PLR_CMB_UNIT_MIN, PLR_CMB_UNIT_MAX);
        }
        cmb.freeze_slow = ase::types::is_in_rng_f(freeze_slow, PLR_CMB_UNIT_MIN, PLR_CMB_UNIT_MAX)
                              ? freeze_slow
                              : PLR_CMB_UNIT_MIN;

        // Stuns: seconds remaining; a positive value holds the figure in place.
        const float landing_stun = hub::get(registry, owner, "CMB_LANDING_STUN"_hs);
        if (!ase::types::is_not_found(landing_stun) && ase::types::is_neg_float(landing_stun)) {
            log::warn(log::WRN::CAT::VALUE_NEGATIVE, "PlayerCmbSynSystem", owner, "CMB_LANDING_STUN",
                      landing_stun);
        }
        cmb.landing_stun = ase::types::is_pos_float(landing_stun) ? landing_stun : PLR_CMB_UNIT_MIN;

        const float patzer_stun = hub::get(registry, owner, "CMB_PATZER_STUN"_hs);
        if (!ase::types::is_not_found(patzer_stun) && ase::types::is_neg_float(patzer_stun)) {
            log::warn(log::WRN::CAT::VALUE_NEGATIVE, "PlayerCmbSynSystem", owner, "CMB_PATZER_STUN",
                      patzer_stun);
        }
        cmb.patzer_stun = ase::types::is_pos_float(patzer_stun) ? patzer_stun : PLR_CMB_UNIT_MIN;

        // Knockback: the row stands only while the flag is set and all three displacements are found.
        const float knbk_active = hub::get(registry, owner, "CMB_KBK_ACTIVE"_hs);
        if (!ase::types::is_not_found(knbk_active) &&
            !ase::types::is_in_rng_f(knbk_active, PLR_CMB_UNIT_MIN, PLR_CMB_UNIT_MAX)) {
            log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "PlayerCmbSynSystem", owner, "CMB_KBK_ACTIVE",
                      knbk_active, PLR_CMB_UNIT_MIN, PLR_CMB_UNIT_MAX);
        }
        if (!ase::types::is_in_rng_f(knbk_active, PLR_CMB_FLAG_SET_MIN, PLR_CMB_UNIT_MAX)) {
            registry.remove<PlayerInpCmbKnbkComponent>(entity);
            continue;  // no knockback runs on this figure
        }
        const float disp_x = hub::get(registry, owner, "CMB_KBK_DISP_X"_hs);
        if (ase::types::is_not_found(disp_x)) {
            registry.remove<PlayerInpCmbKnbkComponent>(entity);
            continue;  // the star sets the flag before the displacement of this frame
        }
        const float disp_y = hub::get(registry, owner, "CMB_KBK_DISP_Y"_hs);
        if (ase::types::is_not_found(disp_y)) {
            registry.remove<PlayerInpCmbKnbkComponent>(entity);
            continue;  // the star sets the flag before the displacement of this frame
        }
        const float disp_z = hub::get(registry, owner, "CMB_KBK_DISP_Z"_hs);
        if (ase::types::is_not_found(disp_z)) {
            registry.remove<PlayerInpCmbKnbkComponent>(entity);
            continue;  // the star sets the flag before the displacement of this frame
        }
        auto& knbk = registry.emplace_or_replace<PlayerInpCmbKnbkComponent>(entity);
        knbk.disp_x = disp_x;
        knbk.disp_y = disp_y;
        knbk.disp_z = disp_z;
    }
}

void PlayerCmbSynSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerCmbSynSystem] Stopped");
}

}  // namespace ase::player
