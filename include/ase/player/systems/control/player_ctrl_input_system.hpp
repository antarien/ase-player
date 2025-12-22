#pragma once
/**
 * PlayerCtrlInputSystem - Process player input from InputStateMoveComponent
 *
 * ECS STATELESS: No private member state!
 *
 * Reads: InputStateMoveComponent (from ase-input)
 * Writes: PlayerStPosComponent (yaw rotation towards movement)
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerCtrlInputSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerCtrlInputSystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
