#pragma once

/**
 * PlayerChunkComponent - Player chunk presence
 *
 * Tracks which chunk the player is currently in.
 * Used for chunk loading/unloading around player.
 * Single responsibility: WHERE in chunk grid.
 */

#include <cstdint>

namespace ase::player {

struct PlayerChunkComponent {
    int32_t chunk_x = 0;
    int32_t chunk_y = 0;
};

}  // namespace ase::player
