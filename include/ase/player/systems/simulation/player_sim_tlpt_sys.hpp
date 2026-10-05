#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_tlpt_sys.hpp
 * @brief       PlayerSimTlptSystem - Applies the backend teleport jump to the player position
 * @description Calc half of the SYN pair for the teleport lever (Phase 13, register D0703):
 *              PlayerSyncTlptSystem mirrors PLR_CHEAT_TELEPORT into PlayerInpTlptComponent, this
 *              system moves the player's chunk-relative position by jump_m each tick — a real
 *              teleport chain on the real player entity, never a fabricated topic inject. No Hub
 *              access here. The velocity is not touched, so the speed authority of
 *              PlayerAccMovSystem stays quiet and its TELEPORT authority (per-tick delta above
 *              PLR_AC_TELEPORT_STEP) is what fires.
 *
 *              It runs before PlayerSimPhysSystem on purpose: the jump lands on local_x, and the
 *              physics step carries any overflow across the cell edge in the same tick, so no reader
 *              ever sees a local coordinate outside [0, edge). The view therefore selects exactly the
 *              players the physics step integrates.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-10-03
 * @modified    2026-10-03
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   PlayerInpTlptComponent.jump_m → PlayerSimTlptSystem → PlayerStaPosComponent.local_x (+ jump)
 *   (bridge, from the Hub lever)    (no Hub access)       → PlayerSimPhysSystem carries the edge
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
 * @brief Applies the backend teleport jump to the player position (calc, no Hub)
 *
 * @schedule Dynamics — after PlayerCtrlMovSystem, before PlayerSimPhysSystem
 * @reads    PlayerInpTlptComponent.jump_m; PlayerInpTrnComponent, PlayerStaVelComponent,
 *           PlayerStaPhysComponent (selection: the players the physics step integrates)
 * @writes   PlayerStaPosComponent.local_x
 */
class PlayerSimTlptSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimTlptSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
