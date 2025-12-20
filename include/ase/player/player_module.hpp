#pragma once

/**
 * PlayerModule - Bevy-style module for player systems
 *
 * Dependency chain (FixedUpdate):
 *   PlayerLifecycleSystem (no deps)
 *   └─ PlayerInputSystem (→ TerrainChunkSystem - cross-module)
 *      └─ PlayerMovementSystem
 *         └─ PlayerPhysicsSystem
 *            └─ PlayerStateSystem
 *               └─ PlayerChunkSystem
 *                  └─ PlayerBroadcastSystem
 */

#include <ase/ecs/app.hpp>
#include <ase/player/systems/player_lifecycle_system.hpp>
#include <ase/player/systems/player_input_system.hpp>
#include <ase/player/systems/player_movement_system.hpp>
#include <ase/player/systems/player_physics_system.hpp>
#include <ase/player/systems/player_state_system.hpp>
#include <ase/player/systems/player_chunk_system.hpp>
#include <ase/player/systems/player_broadcast_system.hpp>

namespace ase::player {

struct PlayerModule {
    void build(ecs::App& app) {
        // FixedUpdate Systems with dependency chain
        app.add_system<PlayerLifecycleSystem>(ecs::Schedule::FixedUpdate);

        app.add_system_with<PlayerInputSystem>(ecs::Schedule::FixedUpdate)
            .run_after("TerrainChunkSystem");  // Cross-module dependency

        app.add_system_with<PlayerMovementSystem>(ecs::Schedule::FixedUpdate)
            .run_after("PlayerInputSystem");

        app.add_system_with<PlayerPhysicsSystem>(ecs::Schedule::FixedUpdate)
            .run_after("PlayerMovementSystem");

        app.add_system_with<PlayerStateSystem>(ecs::Schedule::FixedUpdate)
            .run_after("PlayerPhysicsSystem");

        app.add_system_with<PlayerChunkSystem>(ecs::Schedule::FixedUpdate)
            .run_after("PlayerStateSystem");

        app.add_system_with<PlayerBroadcastSystem>(ecs::Schedule::FixedUpdate)
            .run_after("PlayerChunkSystem");
    }
};

}  // namespace ase::player
