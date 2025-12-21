#pragma once
/**
 * PlayerStateStatusSystem - Update player state machine
 *
 * ECS STATELESS: No private member state!
 *
 * Reads: PlayerStateVelComponent, PlayerStatePhysComponent, InputMovementComponent
 * Writes: PlayerStateStatusComponent
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerStateStatusSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerStateStatusSystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
