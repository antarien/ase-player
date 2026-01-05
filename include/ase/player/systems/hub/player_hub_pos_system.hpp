#pragma once

/**
 * PlayerHubPosSystem - Write player positions to Hub for L4 plugins
 *
 * PLAN_ASE_SDK_V2: L4 plugins read player data via Hub, not direct components
 *
 * Writes: PLR_POS_X, PLR_POS_Y, PLR_POS_Z, PLR_ENTITY_ID per player
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerHubPosSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubPosSystem"; }
    void on_start(ecs::Registry& registry) override;
    void on_stop(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
};

}  // namespace ase::player
