#pragma once
/**
 * PlayerLogCausalitySystem - Logs player causality chain
 *
 * Consistent with SkyLogCausalitySystem and TerrainLogCausalitySystem.
 *
 * ECS STATELESS: Uses PlayerCacheObsComponent for tracking.
 *
 * Reads:
 *   - PlayerMgrTag (manager entity)
 *   - PlayerStIdComponent (all players)
 *   - PlayerStStsComponent (player states)
 *   - PlayerSpawnedTag (just spawned)
 *   - PlayerDirtyTag (position changed)
 *   - PlayerChunkChangedTag (chunk changed)
 *
 * Example output:
 *   [ase-player] [PlayerLogCausalitySystem]
 *   players:3 → idle:1 → walking:1 → running:1 → jumping:0 → dirty:2 → spawned:0
 */

#include <ase/ecs/system.hpp>

namespace ase::player {

class PlayerLogCausalitySystem : public ecs::System {
public:
    const char* name() const override { return "PlayerLogCausalitySystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
