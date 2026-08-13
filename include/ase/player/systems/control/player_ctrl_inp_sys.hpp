#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_ctrl_inp_sys.hpp
 * @brief       PlayerCtrlInpSystem - Process player input and update facing direction
 * @description SHARED System: Reads from PlayerInpExtComponent (no Hub access).
 *              Updates player yaw rotation towards movement direction.
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
 *   PlayerInpExtComponent ──> PlayerCtrlInpSystem ──> PlayerStaPosComponent
 *   (from PlayerSyncInpSystem)  (processes input)       (yaw rotation)
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
 * @brief Process player input and update facing direction (SHARED)
 *
 * @schedule Dynamics - processes input for movement
 * @reads    PlayerInpExtComponent (input bridge from SyncSystem)
 * @reads    PlayerStaIdntComponent, PlayerStaPosComponent, PlayerStMovComponent
 * @writes   PlayerStaPosComponent (yaw rotation towards movement)
 */
class PlayerCtrlInpSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerCtrlInpSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
