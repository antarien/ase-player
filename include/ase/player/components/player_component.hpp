#pragma once

/**
 * PlayerComponent - Core player state stored in ECS
 *
 * Contains position, velocity, and identity.
 * Updated by PlayerSystem each tick.
 *
 * Input handling uses ase::input::InputComponent - see ase-input module.
 */

#include <ase/player/types.hpp>
#include <ase/input/components/input_component.hpp>
#include <string>
#include <chrono>

namespace ase::player {

/**
 * Core player component - position, velocity, and state
 */
struct PlayerComponent {
    // Identity
    std::string player_id;

    // Transform
    Vec3 position{0.0f, 0.0f, 0.0f};  // World position (x, y=up, z)
    float yaw = 0.0f;                  // Rotation around Y axis (radians)

    // Physics
    Vec3 velocity{0.0f, 0.0f, 0.0f};  // Current velocity
    bool on_ground = true;             // Touching terrain

    // State
    PlayerState state = PlayerState::Idle;

    // Timestamps
    std::chrono::steady_clock::time_point last_input;
    std::chrono::steady_clock::time_point spawned_at;

    // Constructor
    PlayerComponent() = default;
    explicit PlayerComponent(const std::string& id)
        : player_id(id)
        , spawned_at(std::chrono::steady_clock::now())
    {}
};

/**
 * Player chunk presence - tracks which chunk player is in
 * Used for chunk loading/unloading around player
 */
struct PlayerChunkPresence {
    int32_t chunk_x = 0;
    int32_t chunk_y = 0;
    bool dirty = true;  // Position changed chunks
};

/**
 * Tag component for player entities that need SSE broadcast
 */
struct PlayerDirty {};

/**
 * Tag component for newly spawned players (broadcast spawn event)
 */
struct PlayerJustSpawned {};

}  // namespace ase::player
