#pragma once
/**
 * PlayerBufBctStaComponent - Buffer for state broadcast serialization
 *
 * Combines PlayerStIdComponent + PlayerStPosComponent + PlayerStVelComponent
 * + PlayerStStsComponent data for serialization.
 *
 * bct = broadcast, sta = state
 */

#include <array>
#include <cstdint>

namespace ase::player {

struct PlayerBufBctStaComponent {
    // From PlayerStIdComponent
    std::array<char, 64> player_id = {};
    uint64_t spawned_at_ms = 0;
    uint64_t last_input_ms = 0;

    // From PlayerStPosComponent
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float yaw = 0.0f;

    // From PlayerStVelComponent
    float vx = 0.0f;
    float vy = 0.0f;
    float vz = 0.0f;

    // From PlayerStStsComponent
    uint8_t state = 0;
};

}  // namespace ase::player
