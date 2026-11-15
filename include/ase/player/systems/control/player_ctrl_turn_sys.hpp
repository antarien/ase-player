#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_ctrl_turn_sys.hpp
 * @brief       PlayerCtrlTurnSystem - turns a standing figure on the spot out of its open course
 * @description The calc half of fig.turn (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.7). It adds every turn input
 *              (PlayerInpTurnComponent) onto the open course of its figure (PlayerReqTurnComponent), then
 *              turns PlayerStaYawComponent out of that course by at most MOVEMENT_DEFAULT_TURN_SPEED per
 *              second and drops the course once it is turned out. A figure that walks takes its heading
 *              from the camera (PlayerCtrlInpSystem); its open course is dropped.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @design      DSGN_030
 * @category    action/control
 * @schedule    Dynamics
 * @parity      server_only
 * @created     2026-11-14
 * @modified    2026-11-14
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * WHY server_only: its inputs, PlayerInpTurnComponent and PlayerReqTurnComponent, exist only where an
 * embedding host files turns; the web client reads the heading from PlayerStaYawComponent.
 *
 * ARCHITECTURE (SYN Pattern - Calc System):
 *
 *   PlayerInpTurnComponent  →PlayerCtrlTurnSystem  →PlayerReqTurnComponent (open course)
 *   (from PlayerHubTurnSystem)  (adds, then turns)    →PlayerStaYawComponent + PlayerDrtyTag
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
 * @brief Turns a standing figure on the spot out of its open course
 *
 * @schedule Dynamics, after PlayerCtrlInpSystem - the walking turn of this tick stands before the turn on
 *           the spot is weighed against it
 * @reads    PlayerInpTurnComponent (request entities), PlayerReqTurnComponent, PlayerStaYawComponent,
 *           PlayerInpExtComponent (whether the figure walks)
 * @writes   PlayerReqTurnComponent (added, turned out, removed), PlayerStaYawComponent, PlayerDrtyTag;
 *           destroys every turn input it took
 */
class PlayerCtrlTurnSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerCtrlTurnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
