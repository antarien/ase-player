#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_ctrl_mov_sys.hpp
 * @brief       PlayerCtrlMovSystem - Calculate player velocity from input
 * @description SHARED System: Reads from PlayerInpExtComponent (no Hub access).
 *              Calculates target velocity based on input and movement settings.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    action/control
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * ARCHITECTURE (SYN Pattern - SHARED Calc System):
 *
 *   PlayerInpExtComponent ──> PlayerCtrlMovSystem ──> PlayerStVelComponent
 *   (from PlayerSyncInpSystem)  (calculates velocity)   (vx, vy, vz)
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
 * @brief Calculate player velocity from input (SHARED)
 *
 * @schedule Dynamics - processes input for movement
 * @reads    PlayerInpExtComponent (input bridge from SyncSystem)
 * @reads    PlayerStPosComponent, PlayerStVelComponent, PlayerStPhysComponent, PlayerStMovComponent
 * @writes   PlayerStVelComponent (velocity from input)
 */
class PlayerCtrlMovSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerCtrlMovSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
