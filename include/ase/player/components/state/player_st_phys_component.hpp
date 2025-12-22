#pragma once
/**
 * PlayerStPhysComponent - Player physics state
 *
 * Contains ground contact and physics flags.
 */

namespace ase::player {

struct PlayerStPhysComponent {
    bool on_ground = true;
    bool gravity_enabled = true;
};

}  // namespace ase::player
