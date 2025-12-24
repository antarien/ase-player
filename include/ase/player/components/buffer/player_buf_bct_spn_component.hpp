#pragma once
/**
 * PlayerBufBctSpnComponent - Buffer for spawn broadcast serialization
 *
 * Combines PlayerStIdComponent + PlayerStPosComponent data for serialization.
 * Used by PlayerNetBctReqSystem to create serialization requests.
 *
 * bct = broadcast, spn = spawn
 */

#include <array>
#include <cstdint>

namespace ase::player {

struct PlayerBufBctSpnComponent {
    // From PlayerStIdComponent
    std::array<char, 64> player_id = {};
    uint64_t spawned_at_ms = 0;
    uint64_t last_input_ms = 0;

    // From PlayerStPosComponent
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float yaw = 0.0f;
};

}  // namespace ase::player
