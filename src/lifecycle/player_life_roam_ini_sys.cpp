/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        player_life_roam_ini_sys.cpp
 * @brief       PlayerLifeRoamIniSystem - Sets a freshly spawned walker on its errand
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    ecs/entity/entitylifecycle
 * @schedule    Dynamics
 * @created     2026-08-30
 * @modified    2026-08-30
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_PLR_LIFE_ROAM_INI: the errand is born with the player)
 *
 *   [PlayerLifeSpwnSystem]
 *          │
 *          │ answers the spawn request with PlayerReqSpwnResComponent on the request entity,
 *          │ where PlayerReqRoamComponent has been riding since the reception system put it there
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: PlayerLifeRoamIniSystem       │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - PlayerReqSpwnResComponent (who)        │
 *   │    - PlayerReqRoamComponent (the errand)    │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - PlayerStaRoamComponent (walkers only)  │
 *   │    - PlayerRoamRestTag (walkers only)       │
 *   │    - PlayerRoamRunTag (fast walkers only)   │
 *   │    - removes PlayerReqRoamComponent         │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ a walker in the RESTING phase, with a first course drawn
 *          ▼
 *   PlayerSimRestSystem decides its first leg when that first pause runs out
 *
 * WHY THIS IS ITS OWN SYSTEM (split from PlayerLifeSpwnSystem, 2026-08-30)
 *
 * The errand block named itself a concern in the origin: "this is the ONE place where a requested
 * speed meets the movement authority". It owns one request component, one state component, two
 * tags, one warning and one log line, and shares none of them with the body creation it sat in.
 *
 * HUB Pattern (MIG_ASE_HUB_API O(1))
 *
 * READS (from Hub):
 *   (none - the errand carries its own speed; the draw sequence is one statement in types.hpp)
 *
 * WRITES (to Hub for other modules):
 *   (none)
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
#include <ase/player/systems/lifecycle/player_life_roam_ini_sys.hpp>
// Components from same module ONLY
#include <ase/player/components/request/player_req_roam_comp.hpp>
#include <ase/player/components/request/player_req_spwn_res_comp.hpp>
#include <ase/player/components/state/player_sta_roam_comp.hpp>
#include <ase/player/components/tag/player_roam_rest_tag.hpp>
#include <ase/player/components/tag/player_roam_run_tag.hpp>
// types.hpp for constants - and for the draw sequence plr_roam_fraction / plr_roam_span
#include <ase/player/types.hpp>
// Logging
#include <ase/log/log.hpp>
// Math
#include <ase/math/math.hpp>

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

// No helper functions needed - the draw sequence a walker's first pause and first course come
// from is ONE statement in types.hpp (plr_roam_fraction, plr_roam_span), shared with the two
// phase systems.

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void PlayerLifeRoamIniSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[PlayerLifeRoamIniSystem] Started");
}

void PlayerLifeRoamIniSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * STEP 1: Every answered spawn request that carried an errand
     *
     * The pair is the seam: the result says WHICH entity was born, the errand says what it was
     * born for. Iterator pattern, because the loop writes components on OTHER entities than the
     * one it iterates and removes one from the iterated entity.
     */
    auto errand_view = registry.view<PlayerReqSpwnResComponent, PlayerReqRoamComponent>();
    auto errand_it = errand_view.begin();
    auto errand_end = errand_view.end();
    while (errand_it != errand_end) {
        auto request_entity = *errand_it;
        ++errand_it;

        const auto& result = registry.get<PlayerReqSpwnResComponent>(request_entity);
        const auto& errand = registry.get<PlayerReqRoamComponent>(request_entity);

        /**
         * STEP 2: Only a request that actually created a player carries a walker
         *
         * The origin set the errand up INSIDE its "player did not exist yet" branch. A request
         * answered with success=false found an existing player; that one keeps whatever errand
         * it already had, and a second one would overwrite a leg in flight.
         */
        auto walker = static_cast<ecs::Entity>(result.spawned_entity);
        if (!result.success || !registry.valid(walker)) {
            log::debug("[PlayerLifeRoamIniSystem] Errand dropped - no player was created");
            registry.remove<PlayerReqRoamComponent>(request_entity);
            continue;
        }

        /**
         * STEP 3: The errand meets the movement authority
         *
         * THE ERRAND IS BORN WITH THE PLAYER, and this is the ONE place where a requested speed
         * meets the movement authority - it is set once, at spawn, and never again. Above the
         * walking gear the errand rides the running one; above the running gear there is no gear
         * left, so the magnitude caps and the request is answered with a warning rather than
         * silently obeyed - a speed beyond the authority is what the cheat lever next door exists
         * for, and a walker that quietly became one would falsify every measurement taken
         * downstream.
         *
         * The walker starts in the RESTING phase: it arrives, stands, and PlayerSimRestSystem
         * decides its first course when that first pause runs out. Setting off mid-stride would
         * need an initial heading decided here as well, and the course of a leg already has
         * exactly one decision maker.
         */
        const uint32_t owner = static_cast<uint32_t>(walker);
        const bool runs = errand.speed > MOVEMENT_DEFAULT_WALK_SPEED;
        const float gear = runs ? MOVEMENT_DEFAULT_RUN_SPEED : MOVEMENT_DEFAULT_WALK_SPEED;
        if (errand.speed > MOVEMENT_DEFAULT_RUN_SPEED) {
            log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "PlayerLifeRoamIniSystem", owner,
                      "errand_speed", errand.speed, 0.0f, MOVEMENT_DEFAULT_RUN_SPEED);
        }
        auto& roam = registry.emplace<PlayerStaRoamComponent>(walker);
        roam.speed = errand.speed;
        roam.forward = math::min(errand.speed / gear, PLR_ROAM_INPUT_ENGAGED);
        roam.heading =
            plr_roam_span(plr_roam_fraction(owner, roam.leg_index, PLR_ROAM_MIX_TURN),
                          0.0f, math::TWO_PI);
        roam.leg_sec =
            plr_roam_span(plr_roam_fraction(owner, roam.leg_index, PLR_ROAM_MIX_REST),
                          PLR_ROAM_REST_MIN_SEC, PLR_ROAM_REST_MAX_SEC);
        registry.emplace<PlayerRoamRestTag>(walker);
        if (runs) {
            registry.emplace<PlayerRoamRunTag>(walker);
        }
        log::info("[PlayerLifeRoamIniSystem] Walker set on an errand at {} m/s "
                  "(input {}, first pause {} s)",
                  roam.speed, roam.forward, roam.leg_sec);

        /**
         * STEP 4: The errand is consumed with the request
         *
         * It travelled WITH the request: leaving it behind would let a later pass read an errand
         * whose player already exists.
         */
        registry.remove<PlayerReqRoamComponent>(request_entity);
    }
}

void PlayerLifeRoamIniSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[PlayerLifeRoamIniSystem] Stopped");
}

}  // namespace ase::player
