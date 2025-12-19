#pragma once

/**
 * PlayerMovementSystem - Calculate velocity from input
 *
 * ECS STATELESS: No private member state!
 *
 * Reads: InputMovementComponent, PlayerPositionComponent, PlayerPhysicsComponent
 * Writes: PlayerVelocityComponent
 *
 * Priority: 71 (after input processing)
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerMovementSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerMovementSystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
