#pragma once
/**
 * PlayerStVelComponent - Player velocity data
 *
 * Contains current velocity vector.
 */

namespace ase::player {

struct PlayerStVelComponent {
    float vx = 0.0f;
    float vy = 0.0f;
    float vz = 0.0f;
};

}  // namespace ase::player
