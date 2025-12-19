#pragma once

/**
 * PlayerInputSystem - Process player input from InputMovementComponent
 *
 * ECS STATELESS: No private member state!
 *
 * Reads: InputMovementComponent (from ase-input)
 * Writes: PlayerPositionComponent (yaw rotation towards movement)
 *
 * Priority: 70 (runs first in player pipeline)
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerInputSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerInputSystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
