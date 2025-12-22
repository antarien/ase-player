#pragma once
/**
 * PlayerStIdComponent - Player identity data
 *
 * Contains player identification and session timestamps.
 */

#include <string>
#include <chrono>

namespace ase::player {

struct PlayerStIdComponent {
    std::string player_id;
    std::chrono::steady_clock::time_point spawned_at;
    std::chrono::steady_clock::time_point last_input;
};

}  // namespace ase::player
