#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_rest_sys.hpp
 * @brief       PlayerSimRestSystem - rests a backend-driven player and decides its next heading
 * @description SERVER-ONLY: the pausing phase of a REAL ase-player entity on an errand. The
 *              forward input is zero while it lasts, and when the rest clock runs out the NEXT
 *              heading is drawn - a course change, never a continuation - and the walker is
 *              handed back to the walking phase. A freshly spawned walker begins here, so it
 *              arrives, stands, and only then sets off.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-08-09
 * @modified    2026-08-09
 * @version     1.0.0
 *
 * ARCHITECTURE (the pausing phase; the leg is PlayerSimWandSystem, the seam is the hub writer):
 *
 *   PlayerStaRoamComponent + PlayerRoamRestTag → PlayerSimRestSystem → zero forward, rest clock,
 *   and at the end of a rest a new heading plus the handover to PlayerRoamWandTag
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
 * @brief The pausing phase of a backend-driven player's journey (SERVER-ONLY)
 *
 * @schedule Dynamics — the errand is decided before the hub writer publishes it in the same tier
 * @reads    PlayerStaRoamComponent, PlayerRoamRestTag
 * @writes   PlayerStaRoamComponent (forward, heading, leg clock, leg counter),
 *           PlayerRoamWandTag / PlayerRoamRestTag on the handover
 */
class PlayerSimRestSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimRestSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
