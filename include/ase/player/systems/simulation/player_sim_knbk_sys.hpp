#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_knbk_sys.hpp
 * @brief       PlayerSimKnbkSystem - moves a knocked back player figure by the displacement of the combat star
 * @description Calc half of the SYN pair for the knockback: PlayerCmbSynSystem lays the three knockback
 *              displacements of the combat star on PlayerInpCmbKnbkComponent while a knockback runs, and
 *              this system adds them to the position of the figure. No Hub access here.
 *
 *              It runs after PlayerCtrlMovSystem and before PlayerSimPhysSystem: the displacement lands
 *              on the local coordinates, and the physics step carries any overflow across the cell edge
 *              in the same tick, so PlayerSimWrldSystem later reads a local coordinate inside [0, edge).
 *              The view therefore selects exactly the figures the physics step integrates.
 *
 *              WHY server_only: the bridge row it reads is a server_only row of this module; the
 *              calculation itself touches no hub API, no ctx resource and no stored entity id.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    process/simulation
 * @parity      server_only
 * @schedule    Dynamics
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-06
 * @modified    2026-11-06
 * @version     1.0.0
 *
 * ARCHITECTURE
 *
 *   PlayerInpCmbKnbkComponent → PlayerSimKnbkSystem → PlayerStaPosComponent
 *   (bridge, from the Hub)      (no Hub access)       local_x += disp_x, local_z += disp_y, y += disp_z
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
 * @brief Moves a knocked back player figure by the displacement of the combat star (calc, no Hub)
 *
 * @schedule Dynamics — after PlayerCtrlMovSystem, before PlayerSimPhysSystem
 * @reads    PlayerInpCmbKnbkComponent; PlayerInpTrnComponent, PlayerStaVelComponent,
 *           PlayerStaPhysComponent (selection: the figures the physics step integrates)
 * @writes   PlayerStaPosComponent.local_x, .local_z, .y
 */
class PlayerSimKnbkSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimKnbkSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
