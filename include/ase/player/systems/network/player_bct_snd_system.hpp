#pragma once

#include <ase/ecs/system.hpp>

namespace ase::player {

/**
 * PlayerBctSndSystem - AUSGEHEND Step 3
 *
 * Sends serialized player broadcasts via ReplicationMsgPndBctTag.
 * Pure ECS pattern: reads SerialJsnFinTag, creates ReplicationMsgDatComponent.
 */
class PlayerBctSndSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerBctSndSystem"; }
    void tick(ecs::Registry& registry, float dt) override;
};

}  // namespace ase::player
