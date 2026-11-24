#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_ctrl_mov_sys.hpp
 * @brief       PlayerCtrlMovSystem - Calculate player velocity from input
 * @description SHARED System: Reads from PlayerInpExtComponent (no Hub access).
 *              Calculates target velocity based on input and movement settings.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    action/control
 * @schedule    Dynamics
 * @parity      shared
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-01-22
 * @modified    2026-11-23
 * @version     00.01.48.00098 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * ARCHITECTURE (SYN Pattern - SHARED Calc System):
 *
 *   PlayerInpExtComponent  →PlayerCtrlMovSystem  →PlayerStaVelWishComponent
 *   (from PlayerSyncInpSystem)  (calculates the walk wish)  (vx, vz)
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
 * @brief Calculate player velocity from input (SHARED)
 *
 * @schedule Dynamics - processes input for movement
 * @reads    PlayerInpExtComponent (input bridge from SyncSystem)
 * @reads    PlayerStaPosComponent, PlayerStaPhysComponent, PlayerStMovComponent
 * @writes   PlayerStaVelWishComponent (walk wish from input; the granted speed PlayerStaVelComponent stays as
 *           PlayerSimStndSystem laid it, B0818)
 */
class PlayerCtrlMovSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerCtrlMovSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
