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
 *                  └─ PlayerNetBctReqSystem (creates serialization requests)
 *
 * Replication schedule:
 *   PlayerNetBctSndSystem (reads serialization results, creates broadcasts)
 *
 * Persistence (Star Citizen Replication Layer Pattern):
 *   PlayerPstSerSystem (serializes + sets ReplicationDtyTag)
 *      └─ ReplicationPstSystem (handles MongoDB via ase-replication)
 *
 * Last (Debug/Logging):
 *   PlayerLogCausalitySystem (→ all systems)
 */

#include <ase/ecs/app.hpp>
#include <ase/player/systems/lifecycle/player_life_spawn_system.hpp>
#include <ase/player/systems/control/player_ctrl_input_system.hpp>
#include <ase/player/systems/control/player_ctrl_move_system.hpp>
#include <ase/player/systems/simulation/player_sim_phys_system.hpp>
#include <ase/player/systems/state/player_state_status_system.hpp>
#include <ase/player/systems/spatial/player_spatial_chunk_system.hpp>
#include <ase/player/systems/network/player_bct_req_system.hpp>
#include <ase/player/systems/network/player_bct_snd_system.hpp>
#include <ase/player/systems/persistence/player_pst_ser_system.hpp>
#include <ase/player/systems/log/player_log_causality_system.hpp>

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

        // Hub: Write positions for L4 plugins (PLAN_ASE_SDK_V2)
        app.add_system_with<PlayerHubPosSystem>(ecs::Schedule::FixedUpdate)
            .run_after("PlayerSpatialChunkSystem");

        // =====================================================================
        // NETWORK: AUSGEHEND (Broadcast to Network)
        // Pure ECS 4-System Pattern
        // =====================================================================
        app.add_system_with<PlayerBctReqSystem>(ecs::Schedule::FixedUpdate) // Step 1
            .run_after("PlayerHubPosSystem");
        app.add_system<PlayerBctSndSystem>(ecs::Schedule::Replication);     // Step 3

        // Persistence (Star Citizen Replication Layer Pattern)
        app.add_system<PlayerPstSerSystem>(ecs::Schedule::Persistence);

        // Last (Debug/Logging) - runs after all other schedules (no .run_after needed)
        app.add_system<PlayerLogCausalitySystem>(ecs::Schedule::Last);
    }
};

}  // namespace ase::player
