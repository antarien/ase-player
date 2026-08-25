#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_pos_sys.hpp
 * @brief       PlayerHubPosSystem - Write player positions to Hub for L4 plugins
 * @description PLAN_ASE_SDK_V2: L4 plugins read player data via Hub, not direct components.
 *              Writes PLR_POS_X, PLR_POS_Y, PLR_POS_Z, PLR_ENTITY_ID per player.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    hub
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * ARCHITECTURE:
 *
 *   PlayerStaPosComponent ──> PlayerHubPosSystem ──> Hub
 *   (position data)         (writes to Hub)        PLR_POS_X, PLR_POS_Y, PLR_POS_Z
 *                                                  PLR_ENTITY_ID
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
 * @brief Write player positions to Hub for L4 plugins
 *
 * @schedule Dynamics - after position updates, in the same tier
 *
 * Corrected 2026-08-20: this system carried TWO different labels - Integration in this header
 * and Dissemination in its .cpp - and player_module.hpp registers it in Dynamics (20), so
 * BOTH were wrong and they were wrong in opposite directions (12 and 70). A reader picking
 * either file would have got a different answer, and neither would have been the running one.
 * Only the registration decides.
 * @reads    PlayerStaPosComponent, PlayerStaIdntComponent
 * @writes   Hub values: PLR_POS_X, PLR_POS_Y, PLR_POS_Z, PLR_ENTITY_ID
 */
class PlayerHubPosSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubPosSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
