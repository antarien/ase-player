#pragma once
/**
 * PlayerModule - Bevy-style module for player systems
 *
 * Dependency chain (FixedUpdate):
 *   PlayerLifeSpawnSystem (no deps)
 *   └─ PlayerCtrlInputSystem (→ TerrainChunkSystem - cross-module)
 *      └─ PlayerCtrlMoveSystem
 *         └─ PlayerSimPhysSystem
 *            └─ PlayerStateStatusSystem
 *               └─ PlayerSpatialChunkSystem
 *                  └─ PlayerNetBroadcastSystem
 */

#include <ase/ecs/app.hpp>
#include <ase/player/systems/lifecycle/player_life_spawn_system.hpp>
#include <ase/player/systems/control/player_ctrl_input_system.hpp>
#include <ase/player/systems/control/player_ctrl_move_system.hpp>
#include <ase/player/systems/simulation/player_sim_phys_system.hpp>
#include <ase/player/systems/state/player_state_status_system.hpp>
#include <ase/player/systems/spatial/player_spatial_chunk_system.hpp>
#include <ase/player/systems/network/player_net_broadcast_system.hpp>

namespace ase::player {

struct PlayerModule {
    static constexpr const char* name() { return "ase-player"; }

    void build(ecs::App& app) {
        app.add_system<PlayerLifeSpawnSystem>(ecs::Schedule::FixedUpdate);

        app.add_system_with<PlayerCtrlInputSystem>(ecs::Schedule::FixedUpdate)
            .run_after("TerrainChunkSystem");

        app.add_system_with<PlayerCtrlMoveSystem>(ecs::Schedule::FixedUpdate)
            .run_after("PlayerCtrlInputSystem");

        app.add_system_with<PlayerSimPhysSystem>(ecs::Schedule::FixedUpdate)
            .run_after("PlayerCtrlMoveSystem");

        app.add_system_with<PlayerStateStatusSystem>(ecs::Schedule::FixedUpdate)
            .run_after("PlayerSimPhysSystem");

        app.add_system_with<PlayerSpatialChunkSystem>(ecs::Schedule::FixedUpdate)
            .run_after("PlayerStateStatusSystem");

        app.add_system_with<PlayerNetBroadcastSystem>(ecs::Schedule::FixedUpdate)
            .run_after("PlayerSpatialChunkSystem");
    }
};

}  // namespace ase::player
