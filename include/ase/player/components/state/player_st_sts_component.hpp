#pragma once
/**
 * PlayerStStsComponent - Player state machine
 *
 * Contains current player state.
 * State values from types.hpp: PLAYER_STATE_IDLE, PLAYER_STATE_WALKING, etc.
 */

#include <cstdint>

namespace ase::player {

struct PlayerStStsComponent {
    uint8_t state = 0;
};

}  // namespace ase::player
