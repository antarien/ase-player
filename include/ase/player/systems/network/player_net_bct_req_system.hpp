#pragma once
/**
 * PlayerNetBctReqSystem - Creates serialization requests for player broadcast
 *
 * Schedule: FixedUpdate (after PlayerSpatialChunkSystem)
 * Reads: PlayerSpawnedTag, PlayerDirtyTag, PlayerChunkChangedTag
 * Creates: SerialBufJsnComponent + SerialJsnPndTag + PlayerBct*PndTag
 *
 * req = request
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerNetBctReqSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerNetBctReqSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
