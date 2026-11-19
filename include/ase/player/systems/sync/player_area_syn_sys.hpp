#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_area_syn_sys.hpp
 * @brief       PlayerAreaSynSystem - lays the plate anchor and the raster edges of every bound figure
 * @description SYN split (WRFL_ASE_SYN_PATTERN): this system does the Hub I/O and nothing else. For every
 *              figure of PlayerSpndTag that carries the row HUB_PLR_AREA it reads the anchor of the bound plate
 *              on the plate itself (TRN_FLAT_HGT, TRN_FLAT_X, TRN_FLAT_Z of ase-terrain) onto
 *              PlayerInpAreaComponent, and the answer ase-gis laid on the figure for the loaded raster around it
 *              (row HUB_GIS_GRND, the chunk borders WEST, EAST, SOUTH, NORTH) onto PlayerInpGrndComponent.
 *              PlayerSimBndySystem reads only the edges, PlayerSimWalkSystem only the anchor.
 *
 *              NO ANSWER IS NO AREA: a figure without the row, without a plate, without a measured anchor or
 *              without a raster answer loses both components instead of keeping last pass's edge. The bound is
 *              the rim of the loaded chunks, never a circle (CAUSA_ASE_GIS, B0541).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    input
 * @parity      server_only
 * @schedule    Ingestion
 * @usecase     usecase:101612 - keep figure in loaded area
 * @actor       Game Areal Plate (AREAL)
 * @created     2026-11-13
 * @modified    2026-11-18
 * @version     00.01.45.00095 [feat]
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
 * @brief Lays the plate anchor and the raster edges of every bound figure
 *
 * @reads    PlayerSpndTag, PlayerStaWrldComponent, row HUB_PLR_AREA with HUB_PLR_AREA_PLATE (Hub, per
 *           figure), TRN_FLAT_HGT/_X/_Z (Hub, owner = the plate, from ase-terrain), row HUB_GIS_GRND with
 *           HUB_GIS_GRND_WEST/_EAST/_SOUTH/_NORTH (Hub, owner = the figure, from ase-gis)
 * @writes   PlayerInpAreaComponent and PlayerInpGrndComponent (on the figure; removed when there is no area),
 *           and takes PlayerAreaBndyTag off together with them: a figure without an area stands on no rim.
 *           No Hub writes.
 */
class PlayerAreaSynSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerAreaSynSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
