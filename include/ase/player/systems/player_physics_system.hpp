#pragma once

/**
 * PlayerPhysicsSystem - Apply gravity and terrain collision
 *
 * ECS STATELESS: No private member state!
 *
 * Reads: PlayerVelocityComponent, terrain height (via TerrainHeightComponent singleton)
 * Writes: PlayerPositionComponent, PlayerPhysicsComponent
 *
 * Priority: 72 (after movement calculation)
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerPhysicsSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerPhysicsSystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
