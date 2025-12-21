#pragma once
/**
 * PlayerReqSpawnResComponent
 *
 * Result of a spawn request.
 * Added by PlayerLifecycleSystem after processing request.
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

struct PlayerReqSpawnResComponent {
    ecs::Entity spawned_entity = ecs::NullEntity;
    bool success = false;
};

}  // namespace ase::player
