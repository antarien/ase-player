#pragma once

/**
 * PlayerBroadcastSystem - Broadcast dirty player entities via WebRTC
 *
 * ECS STATELESS: No private member state!
 *
 * Reads: PlayerDirtyTag, PlayerJustSpawnedTag, all player components
 * Writes: Removes tags after broadcast
 *
 * Uses RTCServer singleton from ase-network for broadcasting.
 *
 * Priority: 75 (last in player pipeline)
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerBroadcastSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerBroadcastSystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
