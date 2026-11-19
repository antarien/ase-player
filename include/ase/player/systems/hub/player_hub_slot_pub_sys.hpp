#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_slot_pub_sys.hpp
 * @brief       PlayerHubSlotPubSystem - puts the seat of its device player on a new figure
 * @description For every host spawn request that was answered with a figure, it lays
 *              PlayerStaSlotComponent on the figure and files the seat as the per-entity hub value
 *              PLR_SEAT. ase-hub keeps that value as the identity of the figure's last state across
 *              a restart (D2467): the entity id changes with every start, the seat does not. The
 *              system computes nothing and reads no hub.
 *              Naming: Player + Hub + Slot(seat of a device player) + Pub(Publish) + System
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:101637 - seat a figure
 * @actor       Player Presence Steward (PLAYPRESTE)
 * @category    hub
 * @schedule    Dynamics
 * @parity      server_only
 * @created     2026-11-19
 * @modified    2026-11-19
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * WHY server_only: the request entities it reads live only in the tier that serves the spawn, and it
 * writes through the hub API.
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
 * @brief Puts the seat of every figure a host spawn request brought into being on that figure
 *
 * @schedule Dynamics - after PlayerLifeSpwnSystem, which answers the request in the same tick
 * @reads    PlayerReqSlotComponent, PlayerReqSpwnResComponent (both on the request entity)
 * @writes   PlayerStaSlotComponent (on the figure), hub value PLR_SEAT (owner = the figure);
 *           takes PlayerReqSlotComponent off the requests it read
 */
class PlayerHubSlotPubSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubSlotPubSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
