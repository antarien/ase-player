#pragma once
/**
 * PlayerStatePosComponent - Player world transform
 *
 * Contains position and rotation.
 * Single responsibility: WHERE is this player.
 */

namespace ase::player {

struct PlayerStatePosComponent {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float yaw = 0.0f;  // Rotation around Y axis (radians)
};

}  // namespace ase::player
