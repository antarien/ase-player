#pragma once

/**
 * PlayerChunkSystem - Track player chunk presence
 *
 * ECS STATELESS: No private member state!
 *
 * Reads: PlayerPositionComponent
 * Writes: PlayerChunkComponent, PlayerChunkChangedTag
 *
 * Priority: 74 (after state updates)
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerChunkSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerChunkSystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
