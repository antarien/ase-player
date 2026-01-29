#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_bct_req_sys.hpp
 * @brief       PlayerBctReqSystem - Create broadcast request entities for player state changes
 * @description Creates serialization entities for players with dirty/spawned tags.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    network/message
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * ARCHITECTURE:
 *
 *   PlayerDirtyTag/PlayerSpawnedTag ──> PlayerBctReqSystem ──> SerialBufJsnComponent
 *   (dirty players)                     (creates requests)     (serialization pending)
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
 * @brief Create broadcast request entities for player state changes
 *
 * @schedule Dynamics - after state updates
 * @reads    PlayerStIdComponent, PlayerStPosComponent, PlayerStVelComponent
 * @reads    PlayerDirtyTag, PlayerSpawnedTag
 * @writes   PlayerBufBctSpnComponent, PlayerBufBctStaComponent, SerialBufJsnComponent
 */
class PlayerBctReqSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerBctReqSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
