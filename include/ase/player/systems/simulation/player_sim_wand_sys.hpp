#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_wand_sys.hpp
 * @brief       PlayerSimWandSystem - walks the current leg of a backend-driven player's journey
 * @description SERVER-ONLY: the walking phase of a REAL ase-player entity the operator sent on an
 *              errand. It holds the heading the errand row carries, keeps the forward magnitude
 *              alive and runs the leg clock down; when the leg ends the walker is handed to the
 *              resting phase. It writes NO velocity and NO position - the ordinary control and
 *              physics systems do that from the input, exactly as they do for a human client.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-08-09
 * @modified    2026-08-09
 * @version     1.0.0
 *
 * ARCHITECTURE (the walking phase; the pause is PlayerSimRestSystem, the seam is the hub writer):
 *
 *   PlayerStaRoamComponent + PlayerRoamWandTag → PlayerSimWandSystem → forward magnitude, leg
 *   clock, and at the end of a leg the handover to PlayerRoamRestTag
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
 * @brief The walking phase of a backend-driven player's journey (SERVER-ONLY)
 *
 * @schedule Dynamics — the errand is decided before the hub writer publishes it in the same tier
 * @reads    PlayerStaRoamComponent, PlayerRoamWandTag
 * @writes   PlayerStaRoamComponent (forward, leg clock), PlayerRoamRestTag / PlayerRoamWandTag on
 *           the handover
 */
class PlayerSimWandSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimWandSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
