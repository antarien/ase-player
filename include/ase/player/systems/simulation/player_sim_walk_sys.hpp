#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_walk_sys.hpp
 * @brief       PlayerSimWalkSystem - files the step the figure wishes to walk, in the frame ase-physics walks it in
 * @description Every frame in Dynamics, after the steering and the rim: every walking figure gets the frame its
 *              place is measured in (PlayerStaFrmComponent) and the placement it follows (PlayerStaPlcmComponent),
 *              and its wish is filed relative to that frame as PlayerBufWalkComponent and
 *              PlayerBufWalkFrmComponent. PlayerHubWalkSystem writes both to the Hub row HUB_PHYS_WALK.
 *
 *              THE FRAME FOLLOWS THE GROUND THE FIGURE STANDS ON. A figure bound to an area plate
 *              (PlayerInpAreaComponent) is measured from the plate centre and the plate level, the frame
 *              ase-physics lays for the plate. A figure on no plate is measured from the corner of its chunk at
 *              level 0, and the frame is laid again once the figure walked more than
 *              PLR_WALK_FRAME_REACH_CHUNKS chunks from it, so the place stays a small float.
 *
 *              A NEW FRAME IS A NEW PLACEMENT. Laying a frame raises the placement count; ase-physics lays the body
 *              at the filed place again, and PlayerSimStndSystem takes no answer to an older count. Teleport,
 *              knockback, migration and the rim mark PlayerWalkRplcTag for the same reason.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @category    process/simulation
 * @schedule    Dynamics
 * @parity      server_only
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
 * @brief Files the step the figure wishes to walk, in the frame ase-physics walks it in
 *
 * @schedule Dynamics - after PlayerSimBndySystem: the step is filed once steering, teleport, knockback and the
 *           rim have set place and velocity of this frame.
 * @reads    PlayerMgrTag, PlayerInpChnkCnstComponent (the chunk edge), PlayerInpAreaComponent,
 *           PlayerStaPosComponent, PlayerStaVelComponent, PlayerStaPhysComponent, PlayerWalkRplcTag
 * @writes   PlayerStaFrmComponent, PlayerStaPlcmComponent, PlayerWalkRplcTag (laid and cleared),
 *           PlayerBufWalkComponent, PlayerBufWalkFrmComponent
 */
class PlayerSimWalkSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimWalkSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
