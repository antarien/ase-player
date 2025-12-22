#pragma once
/**
 * PlayerStChkComponent - Player chunk presence
 *
 * Tracks which chunk the player is currently in.
 * Used for chunk loading/unloading around player.
 */

#include <cstdint>

namespace ase::player {

struct PlayerStChkComponent {
    int32_t chunk_x = 0;
    int32_t chunk_y = 0;
};

}  // namespace ase::player
