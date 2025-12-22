#pragma once
/**
 * PlayerStMovComponent - Movement settings (singleton)
 *
 * Runtime values initialized by system using constexpr from types.hpp.
 * Attached to player manager entity.
 */

#include <cstdint>

namespace ase::player {

struct PlayerStMovComponent {
    // Movement speeds
    float walk_speed = 0.0f;
    float run_speed = 0.0f;
    float jump_impulse = 0.0f;

    // Physics
    float gravity = 0.0f;
    float ground_friction = 0.0f;
    float air_control = 0.0f;
    float ground_snap_dist = 0.0f;

    // Rotation
    float turn_speed = 0.0f;

    // Thresholds
    float min_speed_threshold = 0.0f;
    float velocity_epsilon = 0.0f;

    // Camera/View
    float eye_height = 0.0f;

    // Chunk
    float chunk_size = 0.0f;
};

}  // namespace ase::player
