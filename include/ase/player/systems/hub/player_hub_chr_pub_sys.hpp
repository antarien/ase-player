#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_chr_pub_sys.hpp
 * @brief       PlayerHubChrPubSystem - tells the star which character a new figure plays
 * @description For every spawn request that names a character and was answered with a figure, it
 *              files one row of kind HUB_CRT_CHR with the figure and the hash of the character
 *              (CAUSA_ASE_CREATION, which character a figure plays). ase-creation takes the row and
 *              files the genome and the start values of that character. The system computes
 *              nothing and reads no hub.
 *              Naming: Player + Hub + Chr(Character) + Pub(Publish) + System
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101557 - receive a spawn request
 * @actor       Player Presence Steward (PLAYPRESTE)
 * @category    hub
 * @schedule    Dynamics
 * @parity      server_only
 * @created     2026-11-17
 * @modified    2026-11-17
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * WHY server_only: the request entities it reads live only in the tier that serves the spawn.
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
 * @brief Files the character of every figure a spawn request with a character brought into being
 *
 * @schedule Dynamics - after PlayerLifeSpwnSystem, which answers the request in the same tick
 * @reads    PlayerReqChrComponent, PlayerReqSpwnResComponent (both on the request entity)
 * @writes   rows of kind HUB_CRT_CHR; takes PlayerReqChrComponent off the requests it read
 */
class PlayerHubChrPubSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubChrPubSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
