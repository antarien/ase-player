#pragma once
/**
 * PlayerStateStatusComponent - Player state machine
 *
 * Contains current player state (idle, walking, running, etc.)
 * Single responsibility: WHAT is this player doing.
 */

#include <ase/player/types.hpp>

namespace ase::player {

struct PlayerStateStatusComponent {
    PlayerState state = PlayerState::Idle;
};

}  // namespace ase::player
