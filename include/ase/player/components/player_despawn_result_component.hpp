#pragma once

/**
 * PlayerDespawnResultComponent
 *
 * Result of a despawn request.
 * Added by PlayerLifecycleSystem after processing request.
 */

namespace ase::player {

struct PlayerDespawnResultComponent {
    bool success = false;
};

}  // namespace ase::player
