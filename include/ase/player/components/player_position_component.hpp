#pragma once

/**
 * PlayerPositionComponent - Player world transform
 *
 * Contains position and rotation.
 * Single responsibility: WHERE is this player.
 */

namespace ase::player {

struct PlayerPositionComponent {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float yaw = 0.0f;  // Rotation around Y axis (radians)
};

}  // namespace ase::player
