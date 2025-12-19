#pragma once

/**
 * PlayerDespawnRequestComponent
 *
 * Request to despawn a player entity.
 * Created by REST handlers, processed by PlayerLifecycleSystem.
 */

#include <string>

namespace ase::player {

struct PlayerDespawnRequestComponent {
    std::string player_id;
};

}  // namespace ase::player
