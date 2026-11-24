#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_bndy_sys.hpp
 * @brief       PlayerSimBndySystem - keeps every bound figure inside its loaded area
 * @description Every frame in Dynamics, after the place has been moved and before the chunk address is
 *              stamped: every figure with the edges of its loaded raster (PlayerInpGrndComponent) that stands
 *              beyond a chunk border of that raster less the margin PLR_AREA_EDGE_M is set back onto the border,
 *              each axis on its own, and the part of its horizontal velocity that points outward is taken away.
 *              The outward part of its walk wish (PlayerStaVelWishComponent) is taken as well, so the step
 *              PlayerSimWalkSystem files no longer presses outward. The figure slides along the border instead of
 *              sticking to it. A set back figure gets NO new placement: the glass of ase-physics holds the walking
 *              body at the border, and laying it again would set it at rest outside the box every frame (B0821).
 *              The floor is not here: ase-physics stands the body on the elevation cells ase-gis measured under it.
 *
 *              It runs after PlayerSimTlptSystem and PlayerSimKnbkSystem and before PlayerSimWalkSystem and
 *              PlayerStaChnkSystem on purpose: the teleport jump and the knockback have moved the place for
 *              this frame, the step ase-physics walks is filed from the bounded place, and the chunk address the star sees is
 *              stamped from the bounded place. Chunk address and local metres are written back from the world
 *              place, so no reader sees a local coordinate outside [0, edge), the promise PlayerSimTlptSystem
 *              and PlayerSimWrldSystem rely on.
 *
 *              The edges are a value of the ase-gis raster (HUB_GIS_GRND), never of this module: without the
 *              answer the figure walks unbounded. When chunks stream, the edges move out with the loaded ground.
 *
 *              WHY server_only: the area it reads is filled from the star by a server_only sync system, and the
 *              place it writes is a server_only row of this module.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101612 - keep figure in loaded area
 * @actor       Game Areal Plate (AREAL)
 * @category    process/simulation
 * @schedule    Dynamics
 * @parity      server_only
 * @created     2026-11-13
 * @modified    2026-11-23
 * @version     00.01.48.00098 [feat]
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
 * @brief Keeps every bound figure inside its loaded area
 *
 * @reads    PlayerInpGrndComponent, PlayerInpChnkCnstComponent (manager row), PlayerStaPosComponent,
 *           PlayerStaVelComponent, PlayerStaVelWishComponent
 * @writes   PlayerStaPosComponent (chunk address, local metres), PlayerStaVelComponent (vx, vz),
 *           PlayerStaVelWishComponent (outward vx, vz taken), PlayerAreaBndyTag (set at a border of the raster,
 *           taken off inside it: the rim state); no new placement, the glass of ase-physics holds the body (B0821)
 */
class PlayerSimBndySystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimBndySystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
