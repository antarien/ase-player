#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_tlpt_sys.hpp
 * @brief       PlayerSimTlptSystem - Applies the backend teleport jump to the player position
 * @description Calc half of the SYN pair for the teleport lever (Phase 13, register D0703):
 *              PlayerSyncTlptSystem reads PLR_CHEAT_TELEPORT into PlayerInpTlptComponent in every tick,
 *              this system moves the player's chunk-relative position by jump_m each tick — a real
 *              teleport chain on the real player entity, never a fabricated topic inject. No Hub
 *              access here. The velocity is not touched, so the speed authority of
 *              PlayerAccMovSystem stays quiet and its TELEPORT authority (per-tick delta above
 *              PLR_AC_TELEPORT_STEP) is what fires.
 *
 *              A jumped figure is marked PlayerWalkRplcTag: ase-physics takes the place of the
 *              HUB_PHYS_WALK row only when the placement count moves, so PlayerSimWalkSystem, ordered
 *              after this system, lays the body again at the jumped place. The view selects the figures
 *              with a walking frame (PlayerStaFrmComponent), the ones whose body ase-physics walks.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    process/simulation
 * @schedule    Dynamics
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-10-03
 * @modified    2026-11-14
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * ARCHITECTURE:
 *
 *   PlayerInpTlptComponent.jump_m → PlayerSimTlptSystem → PlayerStaPosComponent.local_x (+ jump)
 *   (bridge, from the Hub lever)    (no Hub access)       → PlayerWalkRplcTag → PlayerSimWalkSystem
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
 * @brief Applies the backend teleport jump to the player position (calc, no Hub)
 *
 * @schedule Dynamics — after PlayerCtrlMovSystem, before PlayerSimBndySystem and PlayerSimWalkSystem
 * @reads    PlayerInpTlptComponent.jump_m; PlayerStaFrmComponent (selection: the figures ase-physics walks)
 * @writes   PlayerStaPosComponent.local_x; PlayerWalkRplcTag
 */
class PlayerSimTlptSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimTlptSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
