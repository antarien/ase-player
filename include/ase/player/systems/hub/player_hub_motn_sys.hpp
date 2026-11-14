#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_motn_sys.hpp
 * @brief       PlayerHubMotnSystem - publishes course, horizontal velocity and gait of every figure as a HUB_PLR_MOTN row
 * @description The motion of a figure on the star. Adopts the hub row kind HUB_PLR_MOTN onto every
 *              entity that carries PlayerSpndTag, fills its fields from course and horizontal velocity
 *              in every pass and publishes the gait tag of this module as one of the three marks of the
 *              kind, so the drawing host animates the figure without including a header of this module.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101611 - animate character motion
 * @actor       SS/PC
 * @category    hub
 * @schedule    Dynamics
 * @parity      server_only
 * @created     2026-11-13
 * @modified    2026-11-13
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   PlayerStaYawComponent  →PlayerHubMotnSystem  →hub row kind HUB_PLR_MOTN
 *   PlayerStaVelComponent   (fills every pass)     (fields HUB_PLR_MOTN_YAW, _VEL_X, _VEL_Y,
 *   PlayerGait*Tag                                  one mark HUB_PLR_GAIT_*; read by ase-pl-avatar)
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
 * @brief Publishes course, horizontal velocity and gait of every figure as a HUB_PLR_MOTN row
 *
 * @schedule Dynamics - after PlayerStaGaitSystem, so the mark states the gait of this frame, and after
 *           PlayerHubSpndSystem, so the figure is on the roster in the same pass.
 * @reads    PlayerSpndTag, PlayerStaYawComponent, PlayerStaVelComponent, PlayerGaitStndTag,
 *           PlayerGaitWalkTag, PlayerGaitRunTag
 * @writes   hub row kind HUB_PLR_MOTN (hub::row_adopt, idempotent), its fields HUB_PLR_MOTN_YAW,
 *           HUB_PLR_MOTN_VEL_X, HUB_PLR_MOTN_VEL_Y (hub::set_f64) and its marks HUB_PLR_GAIT_STND,
 *           HUB_PLR_GAIT_WALK, HUB_PLR_GAIT_RUN (hub::row_tag, hub::row_untag)
 */
class PlayerHubMotnSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubMotnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
