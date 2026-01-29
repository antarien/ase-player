#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_log_obsv_sys.hpp
 * @brief       PlayerLogObsvSystem - Logs player state observation
 * @description Consistent with SkyLogCausalitySystem and TerrainLogCausalitySystem.
 *              ECS STATELESS: Uses PlayerCacheObsComponent for tracking.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    error/logging/output
 * @schedule    Conclusion
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * ARCHITECTURE:
 *
 *   Reads:
 *   - PlayerMgrTag (manager entity)
 *   - PlayerStIdComponent (all players)
 *   - PlayerStStsComponent (player states)
 *   - PlayerSpawnedTag (just spawned)
 *   - PlayerDirtyTag (position changed)
 *   - PlayerChunkChangedTag (chunk changed)
 *
 *   Example output:
 *   [ase-player] [PlayerLogObsvSystem]
 *   players:3 -> idle:1 -> walking:1 -> running:1 -> jumping:0 -> dirty:2 -> spawned:0
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
 * @brief Logs player state observation
 *
 * @schedule Conclusion - logs at end of frame
 * @reads    PlayerMgrTag, PlayerStIdComponent, PlayerStStsComponent, PlayerSpawnedTag,
 *           PlayerDirtyTag, PlayerChunkChangedTag
 * @writes   Log output
 */
class PlayerLogObsvSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerLogObsvSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
