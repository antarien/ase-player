#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_pos_sys.hpp
 * @brief       PlayerHubPosSystem - Write player positions to Hub for L4 plugins
 * @description PLAN_ASE_SDK_V2: L4 plugins read player data via Hub, not direct components.
 *              Copies the world-metre view of every player (PlayerStaWrldComponent) onto PLR_POS_X,
 *              PLR_POS_Y and PLR_POS_Z and writes PLR_ENTITY_ID and PLR_IS_PLAYER per player. It
 *              computes nothing: PlayerSimWrldSystem forms the view in the axis order of the star.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:100049 - publish a hub value
 * @actor       Hub Value Bridge (HUBVAL)
 * @category    hub
 * @schedule    Dynamics
 * @parity      server_only
 * @created     2026-01-22
 * @modified    2026-10-12
 * @version     1.2.0
 *
 * ARCHITECTURE:
 *
 *   PlayerStaWrldComponent  →PlayerHubPosSystem  →Hub
 *   (world-metre view)       (copies to Hub)        PLR_POS_X, PLR_POS_Y, PLR_POS_Z
 *                                                   PLR_ENTITY_ID, PLR_IS_PLAYER
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
 * @brief Write player positions to Hub for L4 plugins
 *
 * @schedule Dynamics - after PlayerSimWrldSystem, in the same tier. The schedule is the one
 *           player_module.hpp registers; a label in a head that differs from the registration
 *           is wrong, and only the registration decides.
 * @reads    PlayerStaWrldComponent, PlayerStaIdntComponent
 * @writes   Hub values: PLR_POS_X, PLR_POS_Y, PLR_POS_Z, PLR_ENTITY_ID, PLR_IS_PLAYER
 */
class PlayerHubPosSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubPosSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
