#pragma once

/**
 * PlayerConfigComponent - Global player configuration (singleton)
 *
 * Contains movement configuration and service references.
 * Attached to a single "player config" entity.
 */

#include <ase/player/types.hpp>
#include <functional>

namespace ase::player {

/**
 * Callback to query terrain height at world position
 */
using HeightQueryFn = std::function<float(float world_x, float world_z)>;

/**
 * Singleton component for global player configuration
 */
struct PlayerConfigComponent {
    MovementConfig movement;
    HeightQueryFn height_query;
};

}  // namespace ase::player
