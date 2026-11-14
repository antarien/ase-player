#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_walk_sys.hpp
 * @brief       PlayerHubWalkSystem - writes the step every walking figure wishes as its HUB_PHYS_WALK row
 * @description SYN split (WRFL_ASE_SYN_PATTERN): this system does the Hub WRITE and nothing else. For every
 *              figure with a filed step (PlayerBufWalkComponent, PlayerBufWalkFrmComponent) and a placement
 *              (PlayerStaPlcmComponent) it adopts the row HUB_PHYS_WALK and fills owner, frame origin, place,
 *              horizontal velocity, jump impulse and placement count. ase-physics walks the body from that row.
 *
 *              A JUMP IS ONE IMPULSE. PlayerCtrlMovSystem marks PlayerJumpReqTag when the figure asks to jump;
 *              this system writes MOVEMENT_DEFAULT_JUMP_IMPULSE for exactly that pass and 0 otherwise, and
 *              clears the tag once written.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    hub
 * @parity      server_only
 * @schedule    Dynamics
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
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
 * @brief Writes the step every walking figure wishes as its HUB_PHYS_WALK row
 *
 * @schedule Dynamics - after PlayerSimWalkSystem filed the step of this frame; ase-physics reads the row in its
 *           next pass.
 * @reads    PlayerBufWalkComponent, PlayerBufWalkFrmComponent, PlayerStaPlcmComponent, PlayerJumpReqTag
 * @writes   row HUB_PHYS_WALK with HUB_PHYS_WALK_OWNER, _ORIGIN_X, _ORIGIN_Z, _X, _Y, _Z, _VEL_X, _VEL_Z, _JUMP,
 *           _SEQ (hub::row_adopt, hub::set_u32, hub::set_f32); PlayerJumpReqTag (cleared)
 */
class PlayerHubWalkSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubWalkSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
