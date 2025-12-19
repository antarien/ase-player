#pragma once

/**
 * PlayerIdentityComponent - Player identity data
 *
 * Contains player identification and session timestamps.
 * Single responsibility: WHO is this player.
 */

#include <string>
#include <chrono>

namespace ase::player {

struct PlayerIdentityComponent {
    std::string player_id;
    std::chrono::steady_clock::time_point spawned_at;
    std::chrono::steady_clock::time_point last_input;
};

}  // namespace ase::player
