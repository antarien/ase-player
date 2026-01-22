#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_state_status_system.hpp
 * @brief       PlayerStateStatusSystem - Update player state machine
 * @description SHARED System: Reads from PlayerInpExtComponent (no Hub access).
 *              Updates player state (idle, walking, running, jumping) based on
 *              velocity and physics state.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state
 * @schedule    Kinematics
 * @created     2026-01-22
 * @modified    2026-01-22
 * @version     1.0.0
 *
 * ARCHITECTURE (SYN Pattern - SHARED Calc System):
 *
 *   PlayerStVelComponent ──────┐
 *   PlayerStPhysComponent ─────┼──> PlayerStateStatusSystem ──> PlayerStStsComponent
 *   PlayerInpExtComponent ─────┘    (state machine)
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
 * @brief Update player state machine (SHARED)
 *
 * @schedule Kinematics - after physics, before state-dependent systems
 * @reads    PlayerStVelComponent, PlayerStPhysComponent, PlayerInpExtComponent
 * @writes   PlayerStStsComponent
 */
class PlayerStateStatusSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerStateStatusSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
