#pragma once
/**
 * PlayerReqDespComponent
 *
 * Request to despawn a player entity.
 * Created by REST handlers, processed by PlayerLifecycleSystem.
 */

#include <string>

namespace ase::player {

struct PlayerReqDespComponent {
    std::string player_id;
};

}  // namespace ase::player
