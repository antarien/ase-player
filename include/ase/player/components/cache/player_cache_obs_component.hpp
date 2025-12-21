#pragma once
/**
 * PlayerCacheObsComponent - Observer system cache (POD)
 *
 * Consistent with SkyCacheObsComponent pattern.
 * Used by PlayerLogCausalitySystem for change detection.
 */

#include <cstdint>

namespace ase::player {

struct PlayerCacheObsComponent {
    uint32_t last_player_count = 0;
    uint32_t last_moving_count = 0;
    float log_interval_timer = 0.0f;
};

}  // namespace ase::player
