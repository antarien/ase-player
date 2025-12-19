#pragma once

/**
 * PlayerSpawnRequestComponent
 *
 * Request to spawn a new player entity.
 * Created by REST handlers, processed by PlayerLifecycleSystem.
 */

#include <string>

namespace ase::player {

struct PlayerSpawnRequestComponent {
    std::string player_id;
    float x = 0.0f;
    float z = 0.0f;
};

}  // namespace ase::player
