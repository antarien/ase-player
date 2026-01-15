#pragma once
/**
 * PlayerCtrlMoveSystem - Calculate velocity from input
 *
 * ECS STATELESS: No private member state!
 *
 * Reads: InputStateMoveComponent, PlayerStPosComponent, PlayerStPhysComponent
 * Writes: PlayerStVelComponent
 */

#include <ase/ecs/system.hpp>

namespace ase::player {

class PlayerCtrlMoveSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerCtrlMoveSystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
