#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_phys_sys.hpp
 * @brief       PlayerSimPhysSystem - Apply physics simulation to player entities
 * @description SHARED System: Reads from PlayerInpExtComponent (no Hub access).
 *              Applies velocity to position and handles ground collision.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * ARCHITECTURE (SYN Pattern - SHARED Calc System):
 *
 *   PlayerStaVelComponent ──> PlayerSimPhysSystem ──> PlayerStaPosComponent
 *   (velocity)                (applies physics)       (position updated)
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
 * @brief Apply physics simulation to player entities (SHARED)
 *
 * @schedule Dynamics - applies physics simulation
 * @reads    PlayerInpExtComponent (terrain height), PlayerStaVelComponent
 * @writes   PlayerStaPosComponent (position), PlayerStaPhysComponent (on_ground)
 */
class PlayerSimPhysSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimPhysSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
