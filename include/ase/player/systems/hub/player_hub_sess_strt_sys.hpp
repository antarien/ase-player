#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_sess_strt_sys.hpp
 * @brief       PlayerHubSessStrtSystem - Announce each player's session start as a one-pass pulse
 * @description Producer of the contract topic PLAYER_SESSION_START (Phase 13, register D0703), the
 *              subscription of CoordinationDetector that had no producer anywhere in the tree. The
 *              engine owns its player entities, so it announces their sessions itself — the trigger
 *              does not depend on a subscribing game emitting it.
 *
 *              A SESSION START IS AN EVENT, NOT A STATE. The Replica turns a raised flag into a
 *              trigger row that re-fires every heartbeat until the flag falls; a flag left at 1
 *              would bill CoordinationDetector every heartbeat for as long as the player plays.
 *              The flag therefore goes up in one pass and down in the next, and each edge leaves in
 *              its own dissemination pass: this system runs in Observation, AFTER Dissemination
 *              (70 < 72, same 1 Hz tier), so a flag raised here goes out in the next pass, and the
 *              lowering in the pass after that.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    hub
 * @schedule    Observation
 * @created     2026-10-03
 * @modified    2026-10-03
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   PlayerStaIdntComponent + PlayerStaPosComponent ──> PlayerHubSessStrtSystem ──> Hub
 *   (a live player, first seen)                        raise: 1 + PlayerSessStrtTag  PLAYER_SESSION_START
 *   PlayerSessStrtTag (raised one pass ago)            lower: 0 + PlayerSessDoneTag  (owner = entity)
 *
 * ECS SYSTEM HEADER COMPLIANCE
 *
 * [ ] STATELESS - No member variables
 * [ ] Views created on demand, not stored
 * [ ] NO direct calls to other systems
 * [ ] Communication only via Components
 * [ ] Helpers in anonymous namespace (in .cpp, NOT static functions!)
 * [ ] Math functions from ase-math (Layer 0)
 * [ ] NO file-level static/constexpr (constants → types.hpp)
 * [ ] Registered in Module with correct Schedule
 * [ ] Filename matches convention
 * [ ] Class name derived from filename
 * [ ] ALL THREE METHODS DECLARED: on_start, tick, on_stop
 */

#include <ase/ecs/system.hpp>

namespace ase::player {

/**
 * @brief Raises PLAYER_SESSION_START for a new player for exactly one dissemination pass
 *
 * @schedule Observation - after Dissemination in the same 1 Hz tier, so each edge leaves alone
 * @reads    PlayerStaIdntComponent, PlayerStaPosComponent, PlayerSessStrtTag, PlayerSessDoneTag
 * @writes   Hub "PLAYER_SESSION_START" (owner = player entity), PlayerSessStrtTag, PlayerSessDoneTag
 */
class PlayerHubSessStrtSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubSessStrtSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
