#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_stnd_sys.hpp
 * @brief       PlayerSimStndSystem - takes the place ase-physics walked a figure to back into its chunk address
 * @description Every frame in Dynamics, before the steering: every walking figure whose answer of ase-physics
 *              (PlayerInpStndComponent) follows its current placement turns the place of the answer, given
 *              relative to the frame the figure filed (PlayerStaFrmComponent), back into chunk address, local
 *              metres and height, takes the vertical speed of the answer and states its ground contact from
 *              PlayerStndGrndTag. Gravity, ground and falling are values of ase-physics alone; this system
 *              computes none of them, it only changes the frame of the numbers.
 *
 *              AN OLDER ANSWER IS NEVER TAKEN. An answer whose placement count is not the count of the figure
 *              follows a place the figure no longer stands at (teleport, knockback, migration, the rim); it is
 *              skipped until ase-physics has walked the new placement. A figure marked PlayerWalkRplcTag is
 *              being placed in this frame and takes no answer either.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @category    process/simulation
 * @schedule    Dynamics
 * @parity      shared
 * @created     2026-11-14
 * @modified    2026-11-14
 * @version     1.0.0
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
 * @brief Takes the place ase-physics walked a figure to back into its chunk address
 *
 * @schedule Dynamics - first of the figure's motion: the steering reads the ground contact of this answer.
 * @reads    PlayerMgrTag, PlayerInpChnkCnstComponent (the chunk edge), PlayerInpStndComponent,
 *           PlayerStaFrmComponent, PlayerStaPlcmComponent, PlayerStndGrndTag, PlayerWalkRplcTag (excluded)
 * @writes   PlayerStaPosComponent, PlayerStaVelComponent.vy, PlayerStaPhysComponent.on_ground, PlayerDrtyTag
 */
class PlayerSimStndSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimStndSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
