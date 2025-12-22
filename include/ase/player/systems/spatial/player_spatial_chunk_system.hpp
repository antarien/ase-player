#pragma once
/**
 * PlayerSpatialChunkSystem - Track player chunk presence
 *
 * ECS STATELESS: No private member state!
 *
 * Reads: PlayerStPosComponent
 * Writes: PlayerStChkComponent, PlayerChunkChangedTag
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerSpatialChunkSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSpatialChunkSystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
