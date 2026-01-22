#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_bct_req_system.hpp
 * @brief       PlayerBctReqSystem - AUSGEHEND Step 1
 * @description Creates serialization requests for player broadcast.
 *              Pure ECS pattern: copies state to buffer, sets SerialJsnPndTag.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    network
 * @schedule    Integration
 * @created     2026-01-22
 * @modified    2026-01-22
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   PlayerDirtyTag ───┐
 *   PlayerSpawnedTag ─┼──> PlayerBctReqSystem ──> SerialBufJsnComponent
 *   PlayerState* ─────┘    (creates request)      + SerialJsnPndTag
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
 * @brief Creates serialization requests for player broadcast
 *
 * @schedule Integration - prepares broadcast data
 * @reads    PlayerDirtyTag, PlayerSpawnedTag, PlayerState components
 * @writes   SerialBufJsnComponent, SerialJsnPndTag
 */
class PlayerBctReqSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerBctReqSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
