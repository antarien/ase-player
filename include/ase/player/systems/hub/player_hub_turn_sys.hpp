#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_turn_sys.hpp
 * @brief       PlayerHubTurnSystem - lays out the turn rows of an embedding host as turn inputs
 * @description Takes every row HUB_PLR_TURN_REQ that ase-pl-avatar files for a turn on the spot
 *              (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.7, fig.turn), lays it out as PlayerInpTurnComponent on
 *              a request entity of its own and releases the row whatever its outcome. The sync half of the
 *              SYN pattern: it reads the hub and computes nothing.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101554 - receive player input
 * @actor       Player Input Warden (PLAYINPWAR)
 * @design      DSGN_021
 * @category    hub
 * @schedule    Reception
 * @parity      server_only
 * @created     2026-11-14
 * @modified    2026-11-14
 * @version     1.0.0
 *
 * WHY server_only: only an embedding host files turn rows; the web client turns no figure through them.
 *
 * ARCHITECTURE:
 *
 *   ase-pl-avatar          →PlayerHubTurnSystem        →PlayerInpTurnComponent on a request entity
 *   (row HUB_PLR_TURN_REQ,   (reads the row, releases     (taken by PlayerCtrlTurnSystem in
 *    mark HUB_PLR_TURN)       it whatever its outcome)     Dynamics of the same tick)
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
 * @brief Lays out the turn rows of an embedding host as turn inputs
 *
 * @schedule Reception - before PlayerCtrlTurnSystem takes the inputs in Dynamics of the same tick
 * @reads    hub rows HUB_PLR_TURN_REQ (HUB_PLR_TURN_FIGURE, HUB_PLR_TURN_YAW)
 * @writes   PlayerInpTurnComponent on a new request entity per row; releases the row
 */
class PlayerHubTurnSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubTurnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
