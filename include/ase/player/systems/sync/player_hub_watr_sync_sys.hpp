#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_watr_sync_sys.hpp
 * @brief       PlayerHubWatrSyncSystem - lays the water depth under every figure onto the figure
 * @description SYN split (WRFL_ASE_SYN_PATTERN): this system does the Hub READ and nothing else. For every
 *              figure on the star (PlayerSpndTag, row HUB_PLR_SPND) it reads GIS_HYD_CELL_DEPTH, which
 *              GisQryHydHubWrtSystem of ase-gis writes on that figure from the place it filed, into
 *              PlayerInpWatrComponent. PlayerStaStsSystem reads only that component (CAUSA_ASE_HYDRO,
 *              "Wasser je Ort", item 6; B0358).
 *
 *              A DEPTH NOT MEASURED IS NO DEPTH: a figure whose cell is no longer measured loses the
 *              component instead of keeping last pass's water.
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
 * @brief Lays the water depth under every figure onto the figure
 *
 * @schedule Reception - the depth ase-gis filed in the last tick is taken before PlayerStaStsSystem reads
 *           it in Dynamics.
 * @reads    GIS_HYD_CELL_DEPTH on the figure entity (hub::is_measured, hub::get)
 * @writes   PlayerInpWatrComponent (on the figure; removed when the depth is no longer measured)
 */
class PlayerHubWatrSyncSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubWatrSyncSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
