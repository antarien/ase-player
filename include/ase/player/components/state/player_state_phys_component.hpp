#pragma once
/**
 * PlayerStatePhysComponent - Player physics state
 *
 * Contains ground contact and physics flags.
 * Single responsibility: Physics collision state.
 */

namespace ase::player {

struct PlayerStatePhysComponent {
    bool on_ground = true;
    bool gravity_enabled = true;
};

}  // namespace ase::player
