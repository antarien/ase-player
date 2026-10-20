#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_spnd_sys.hpp
 * @brief       PlayerHubSpndSystem - publishes every figure in the world as a HUB_PLR_SPND row
 * @description The figure roster of the star. Adopts the hub row kind HUB_PLR_SPND onto every
 *              entity that carries PlayerSpndTag, so foreign modules walk the figures through the
 *              hub API and include no header of this module.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:5130 - play a character
 * @actor       SS/PC
 * @category    hub
 * @schedule    Dynamics
 * @parity      server_only
 * @created     2026-10-13
 * @modified    2026-10-13
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   PlayerSpndTag       →PlayerHubSpndSystem  →hub row kind HUB_PLR_SPND
 *   (set by the spawn    (adopts the row)       (read by bdi, camera, character,
 *    and the migration)                          geoid and world)
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
 * @brief Publishes every figure in the world as a HUB_PLR_SPND row
 *
 * @schedule Dynamics - after PlayerLifeSpwnSystem, so a figure spawned this tick is a row in the
 *           same tick; a migrated figure arrives in Reception, earlier in the same tick.
 * @reads    PlayerSpndTag
 * @writes   hub row kind HUB_PLR_SPND (hub::row_adopt, idempotent)
 */
class PlayerHubSpndSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubSpndSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
