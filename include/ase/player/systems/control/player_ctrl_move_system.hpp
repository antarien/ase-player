#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_ctrl_move_system.hpp
 * @brief       PlayerCtrlMoveSystem - Calculate velocity from input
 * @description Converts input state to player velocity based on physics properties.
 *              ECS STATELESS: No private member state!
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    control
 * @schedule    Reception
 * @created     2026-01-22
 * @modified    2026-01-22
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   InputStateMoveComponent ───┐
 *   PlayerStPosComponent ──────┼──> PlayerCtrlMoveSystem ──> PlayerStVelComponent
 *   PlayerStPhysComponent ─────┘   (calculates velocity)
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
 * @brief Calculate velocity from input
 *
 * @schedule Reception - processes input at start of frame
 * @reads    InputStateMoveComponent, PlayerStPosComponent, PlayerStPhysComponent
 * @writes   PlayerStVelComponent
 */
class PlayerCtrlMoveSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerCtrlMoveSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
