#pragma once

/**
 * PlayerStateSystem - Update player state machine
 *
 * ECS STATELESS: No private member state!
 *
 * Reads: PlayerVelocityComponent, PlayerPhysicsComponent, InputComponent
 * Writes: PlayerStateComponent
 *
 * Priority: 73 (after physics)
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerStateSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerStateSystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
