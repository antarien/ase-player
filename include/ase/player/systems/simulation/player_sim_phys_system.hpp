#pragma once
/**
 * PlayerSimPhysSystem - Apply gravity and terrain collision
 *
 * ECS STATELESS: No private member state!
 *
 * Reads: PlayerStateVelComponent, terrain height (via PlayerStateCfgComponent singleton)
 * Writes: PlayerStatePosComponent, PlayerStatePhysComponent
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerSimPhysSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimPhysSystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
