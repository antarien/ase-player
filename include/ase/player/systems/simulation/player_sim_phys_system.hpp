#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_phys_system.hpp
 * @brief       PlayerSimPhysSystem - Apply gravity and terrain collision
 * @description SHARED System: Reads from PlayerInpExtComponent (no Hub access).
 *              Processes physics simulation for players including gravity and
 *              terrain collision detection.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    simulation
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-22
 * @version     1.0.0
 *
 * ARCHITECTURE (SYN Pattern - SHARED Calc System):
 *
 *   PlayerStVelComponent ─────┐
 *   PlayerInpExtComponent ────┼──> PlayerSimPhysSystem ──> PlayerStPosComponent
 *   (terrain height)          │    (physics sim)           PlayerStPhysComponent
 *
 * ECS SYSTEM HEADER COMPLIANCE
 *
 * [ ] STATELESS - No member variables
 * [ ] Views created on demand, not stored
 * [ ] NO direct calls to other systems
 * [ ] Communication only via Components
 * [ ] Helpers in anonymous namespace (in .cpp, NOT static functions!)
 * [ ] Math functions from ase-math (Layer 0)
 * [ ] NO file-level static/constexpr (constants -> types.hpp)
 * [ ] Registered in Module with correct Schedule
 * [ ] Filename matches convention
 * [ ] Class name derived from filename
 * [ ] ALL THREE METHODS DECLARED: on_start, tick, on_stop
 */

#include <ase/ecs/system.hpp>

namespace ase::player {

/**
 * @brief Apply gravity and terrain collision (SHARED)
 *
 * @schedule Dynamics - physics simulation
 * @reads    PlayerStVelComponent, PlayerInpExtComponent (trn_hgt)
 * @writes   PlayerStPosComponent, PlayerStPhysComponent
 */
class PlayerSimPhysSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimPhysSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
