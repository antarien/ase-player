#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_spwn_sys.hpp
 * @brief       PlayerHubSpwnSystem - turns the spawn rows of an embedding host into spawn requests
 * @description Takes every row HUB_PLR_SPWN_REQ and files the PlayerReqSpwnComponent request the
 *              network reception files for a spawn frame, so PlayerLifeSpwnSystem serves both entries
 *              alike. Destroys its own answered requests in the next pass.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101557 - receive a spawn request
 * @actor       Player Presence Steward (PLAYPRESTE)
 * @category    hub
 * @schedule    Reception
 * @parity      server_only
 * @created     2026-11-13
 * @modified    2026-11-13
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * ARCHITECTURE:
 *
 *   ase-pl-avatar          →PlayerHubSpwnSystem        →PlayerReqSpwnComponent + PlayerSpwnHubTag
 *   (row HUB_PLR_SPWN_REQ,   (reads the row, releases     (served by PlayerLifeSpwnSystem in
 *    mark HUB_PLR_SPWN)       it whatever its outcome)     Dynamics of the same tick)
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
 * @brief Turns the spawn rows of an embedding host into spawn requests
 *
 * @schedule Reception - like the network reception PlayerSpwnRcvSystem, before PlayerLifeSpwnSystem
 *           serves the request in Dynamics of the same tick
 * @reads    hub rows HUB_PLR_SPWN_REQ (HUB_PLR_SPWN_SEAT, HUB_PLR_SPWN_X, HUB_PLR_SPWN_Z),
 *           PlayerReqSpwnResComponent + PlayerSpwnHubTag (its answered requests)
 * @writes   PlayerReqSpwnComponent + PlayerSpwnHubTag on a new request entity; releases the row
 */
class PlayerHubSpwnSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubSpwnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
