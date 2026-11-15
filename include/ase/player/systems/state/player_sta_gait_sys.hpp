#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sta_gait_sys.hpp
 * @brief       PlayerStaGaitSystem - set the gait tag of every player figure from its horizontal speed
 * @description SHARED System: reads PlayerStaVelComponent and moves each figure onto exactly one of
 *              PlayerGaitStndTag, PlayerGaitWalkTag, PlayerGaitRunTag (no Hub access).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state
 * @schedule    Dynamics
 * @parity      shared
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-13
 * @modified    2026-11-13
 * @version     00.00.00.00000 [seed]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * ARCHITECTURE (SYN Pattern - SHARED Calc System):
 *
 *   PlayerStaVelComponent  →PlayerStaGaitSystem  →PlayerGaitStndTag | PlayerGaitWalkTag | PlayerGaitRunTag
 *   (velocity)              (speed → gait)          (exactly one per figure)
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
 * @brief Set the gait tag of every player figure from its horizontal speed (SHARED)
 *
 * @schedule Dynamics - after PlayerSimStndSystem has taken the velocity of this tick
 * @reads    PlayerStaVelComponent, PlayerStaIdntComponent
 * @writes   PlayerGaitStndTag, PlayerGaitWalkTag, PlayerGaitRunTag
 */
class PlayerStaGaitSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerStaGaitSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
