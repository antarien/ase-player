#pragma once
/**
 * PlayerStPosComponent - Player world transform
 *
 * Contains position and rotation.
 */

namespace ase::player {

struct PlayerStPosComponent {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float yaw = 0.0f;
};

}  // namespace ase::player
