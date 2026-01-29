#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sta_chnk_sys.hpp
 * @brief       PlayerStaChnkSystem - Track player chunk position for streaming
 * @description Updates chunk component when player position changes chunks.
 *              ECS STATELESS: No private member state!
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * ARCHITECTURE:
 *
 *   PlayerStPosComponent ──> PlayerStaChnkSystem ──> PlayerStChkComponent
 *   (position)              (tracks chunks)         PlayerChunkChangedTag
 *
 * ECS SYSTEM HEADER COMPLIANCE
 *
 * [ ] STATELESS - No member variables
 * [ ] Views created on demand, not stored
 * [ ] NO direct calls to other systems
 * [ ] Communication only via Components
 * [ ] Helpers in anonymous namespace (in .cpp, NOT static functions!)
 * [ ] Math functions from ase-math (Layer 0)
 * [ ] NO file-level static/constexpr (constants -> types.hpp)
 * [ ] Registered in Module with correct Schedule
 * [ ] Filename matches convention
 * [ ] Class name derived from filename
 * [ ] ALL THREE METHODS DECLARED: on_start, tick, on_stop
 */

#include <ase/ecs/system.hpp>

namespace ase::player {

/**
 * @brief Track player chunk presence
 *
 * @schedule Dynamics - after position updates
 * @reads    PlayerStPosComponent
 * @writes   PlayerStChkComponent, PlayerChunkChangedTag
 */
class PlayerStaChnkSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerStaChnkSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
