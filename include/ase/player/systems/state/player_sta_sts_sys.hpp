#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sta_sts_sys.hpp
 * @brief       PlayerStaStsSystem - Determine player movement state from velocity and physics
 * @description SHARED System: Reads from PlayerInpExtComponent (no Hub access).
 *              Determines state (idle, walking, running, jumping, falling).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state/lifecycle/activity
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * ARCHITECTURE (SYN Pattern - SHARED Calc System):
 *
 *   PlayerStVelComponent ──> PlayerStaStsSystem ──> PlayerStStsComponent
 *   (velocity)               (determines state)      (state updated)
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
 * @brief Determine player movement state (SHARED)
 *
 * @schedule Dynamics - determines state after physics
 * @reads    PlayerInpExtComponent, PlayerStVelComponent, PlayerStPhysComponent
 * @writes   PlayerStStsComponent (state value)
 */
class PlayerStaStsSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerStaStsSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
