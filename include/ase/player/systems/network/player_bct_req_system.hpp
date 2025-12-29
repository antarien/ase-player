#pragma once

#include <ase/ecs/ecs.hpp>

namespace ase::player {

/**
 * PlayerBctReqSystem - AUSGEHEND Step 1
 *
 * Creates serialization requests for player broadcast.
 * Pure ECS pattern: copies state to buffer, sets SerialJsnPndTag.
 */
class PlayerBctReqSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerBctReqSystem"; }
    void on_start(ecs::Registry& registry) override;
    void on_stop(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
};

}  // namespace ase::player
