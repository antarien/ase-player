#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_stnd_sync_sys.hpp
 * @brief       PlayerHubStndSyncSystem - lays the answer of ase-physics on every walking figure
 * @description SYN split (WRFL_ASE_SYN_PATTERN): this system does the Hub READ and nothing else. For every
 *              figure that carries the row HUB_PHYS_STAND it reads place, vertical speed and placement count
 *              into PlayerInpStndComponent, the granted level speed into PlayerInpStndVlcyComponent, and states
 *              the mark HUB_PHYS_STAND_GRND as PlayerStndGrndTag. PlayerSimStndSystem reads only those three.
 *
 *              AN ANSWER NOT FILED IS NO ANSWER: a figure whose row was withdrawn, or whose row lacks a field,
 *              loses the component instead of keeping last pass's place.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    input
 * @parity      server_only
 * @schedule    Reception
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-14
 * @modified    2026-11-14
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
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
 * @brief Lays the answer of ase-physics on every walking figure
 *
 * @schedule Reception - the answer ase-physics filed in Collision of the last tick is taken before
 *           PlayerSimStndSystem reads it in Dynamics.
 * @reads    row HUB_PHYS_STAND with HUB_PHYS_STAND_X, _Y, _Z, _VEL_X, _VEL_Y, _VEL_Z, _SEQ (hub::row_each,
 *           hub::try_get_f32, hub::try_get_u32) and its mark HUB_PHYS_STAND_GRND (hub::row_each with the mark)
 * @writes   PlayerInpStndComponent, PlayerInpStndVlcyComponent, PlayerStndGrndTag (on the figure; all removed with
 *           the answer)
 */
class PlayerHubStndSyncSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubStndSyncSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
