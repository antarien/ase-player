#pragma once

/**
 * ASE MODULE DEFINITION
 *
 * @file        player_module.hpp
 * @brief       Module registration for ase-player
 * @description Registers all systems with the App scheduler (Bevy-style).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @created     2025-12-01
 * @modified    2026-01-22
 * @version     2.0.0
 *
 * ECS MODULE/PLUGIN DEFINITION COMPLIANCE
 *
 * [ ] name() returns correct name (ase-{module} or ase-pl-{plugin})
 * [ ] build() registers all systems in correct schedules
 * [ ] Startup systems registered first (run once at start)
 * [ ] Initialization systems registered (entity creation)
 * [ ] Integration/FixedUpdate systems registered with run_after() ordering
 * [ ] Replication/Transmission systems registered (network sync)
 * [ ] Persistence systems registered (database writes)
 * [ ] Shutdown systems registered (cleanup)
 * [ ] All system includes present
 * [ ] No circular dependencies
 */

#include <ase/ecs/app.hpp>
#include <ase/player/version.hpp>
#include <ase/player/systems/lifecycle/player_life_spwn_sys.hpp>
#include <ase/player/systems/control/player_ctrl_inp_sys.hpp>
#include <ase/player/systems/control/player_ctrl_mov_sys.hpp>
#include <ase/player/systems/simulation/player_sim_phys_sys.hpp>
#include <ase/player/systems/state/player_sta_sts_sys.hpp>
#include <ase/player/systems/state/player_sta_chnk_sys.hpp>
#include <ase/player/systems/hub/player_hub_pos_sys.hpp>
#include <ase/player/systems/persistence/player_pst_ser_sys.hpp>
#include <ase/player/systems/log/player_log_obsv_sys.hpp>
#include <ase/player/systems/sync/player_sync_inp_sys.hpp>

namespace ase::player {

/**
 * @brief PlayerModule - Player entity management module
 * DESIGN_PLAYER: Handles player lifecycle, movement, physics, state, and persistence.
 *
 * Systems: 9 total
 * Dependencies: ase-input, ase-terrain (via Hub)
 */
struct PlayerModule {

    static constexpr const char* name() { return "ase-player"; }
    static constexpr const char* version() { return MODULE_VERSION; }

    void build(ecs::App& app) {

        /**
         * DYNAMICS (Schedule::Dynamics, 30Hz)
         * Physics, movement, game logic systems.
         * Order matters - use run_after() for dependencies.
         */
        app.add_system<PlayerLifeSpwnSystem>(ecs::Schedule::Dynamics);

        app.add_system_with<PlayerCtrlInpSystem>(ecs::Schedule::Dynamics)
            .run_after("TerrainChunkSystem");

        app.add_system_with<PlayerCtrlMovSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerCtrlInpSystem");

        app.add_system_with<PlayerSimPhysSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerCtrlMovSystem");

        app.add_system_with<PlayerStaStsSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimPhysSystem");

        app.add_system_with<PlayerStaChnkSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerStaStsSystem");

        app.add_system_with<PlayerHubPosSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerStaChnkSystem");

        /**
         * PRESERVATION (Schedule::Preservation, 1Hz)
         * Save state to MongoDB.
         */
        app.add_system<PlayerPstSerSystem>(ecs::Schedule::Preservation);

        /**
         * CONCLUSION (Schedule::Conclusion)
         * Debug/logging runs after all other schedules.
         */
        app.add_system<PlayerLogObsvSystem>(ecs::Schedule::Conclusion);

        /**
         * INTEGRATION (Schedule::Integration)
         * Sync input state from network.
         */
        app.add_system<PlayerSyncInpSystem>(ecs::Schedule::Integration);
    }
};

}  // namespace ase::player
