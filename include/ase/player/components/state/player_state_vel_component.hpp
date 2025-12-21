#pragma once
/**
 * PlayerStateVelComponent - Player velocity data
 *
 * Contains current velocity vector.
 * Single responsibility: HOW FAST is this player moving.
 */

namespace ase::player {

struct PlayerStateVelComponent {
    float vx = 0.0f;
    float vy = 0.0f;
    float vz = 0.0f;
};

}  // namespace ase::player
