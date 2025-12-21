#pragma once
/**
 * PlayerReqDespResComponent
 *
 * Result of a despawn request.
 * Added by PlayerLifecycleSystem after processing request.
 */

namespace ase::player {

struct PlayerReqDespResComponent {
    bool success = false;
};

}  // namespace ase::player
