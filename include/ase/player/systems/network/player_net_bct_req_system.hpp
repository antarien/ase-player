#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_net_bct_req_system.hpp
 * @brief       PlayerNetBctReqSystem - Creates serialization requests for player broadcast
 * @description Schedule: Dynamics (after PlayerSpatialChunkSystem)
 *              req = request
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    network
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-22
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   PlayerSpawnedTag ─────────┐
 *   PlayerDirtyTag ───────────┼──> PlayerNetBctReqSystem ──> SerialBufJsnComponent
 *   PlayerChunkChangedTag ────┘    (creates request)         + SerialJsnPndTag
 *                                                            + PlayerBct*PndTag
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
 * @schedule Dynamics - after PlayerSpatialChunkSystem
 * @reads    PlayerSpawnedTag, PlayerDirtyTag, PlayerChunkChangedTag
 * @writes   SerialBufJsnComponent, SerialJsnPndTag, PlayerBct*PndTag
 */
class PlayerNetBctReqSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerNetBctReqSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
