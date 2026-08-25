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
 * @design      DSGN_021
 * @design      DSGN_030
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
 * [ ] Integration/Dynamics systems registered with run_after() ordering
 * [ ] Transmission systems registered (network sync)
 * [ ] Preservation systems registered (database writes)
 * [ ] Finalization systems registered (cleanup)
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
#include <ase/player/systems/hub/player_hub_roam_sys.hpp>
#include <ase/player/systems/hub/player_hub_sess_reg_sys.hpp>
#include <ase/player/systems/persistence/player_pst_ser_sys.hpp>
#include <ase/player/systems/log/player_log_obsv_sys.hpp>
#include <ase/player/systems/sync/player_sync_inp_sys.hpp>
#include <ase/player/systems/anticheat/player_acc_mov_sys.hpp>
#include <ase/player/systems/anticheat/player_acc_act_sys.hpp>
#include <ase/player/systems/anticheat/player_acc_cmb_sys.hpp>
#include <ase/player/systems/anticheat/player_acc_com_sys.hpp>
#include <ase/player/systems/anticheat/player_acc_eco_sys.hpp>
#include <ase/player/systems/anticheat/player_acc_inv_sys.hpp>
#include <ase/player/systems/simulation/player_sim_cht_sys.hpp>
#include <ase/player/systems/simulation/player_sim_wand_sys.hpp>
#include <ase/player/systems/simulation/player_sim_rest_sys.hpp>
#include <ase/player/systems/simulation/player_sim_act_sys.hpp>
#include <ase/player/systems/simulation/player_sim_com_sys.hpp>
#include <ase/player/systems/simulation/player_sim_eco_sys.hpp>
#include <ase/player/systems/reception/player_spwn_rcv_sys.hpp>
#include <ase/player/systems/migration/player_mig_des_sys.hpp>
#include <ase/player/systems/migration/player_mig_ser_sys.hpp>

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
         * RECEPTION (Schedule::Reception)
         * Drain the Replica-forwarded backend spawn frame (BIN_MSG_PLAYER_SPAWN=77, LANE_SPW) and create a
         * PlayerReqSpwnComponent request. Runs in Reception (before Dynamics) so PlayerLifeSpwnSystem spawns
         * the entity the SAME tick — a backend-driven spawn with no human WebRTC client (Phase 13 Task 13.10).
         */
        app.add_system<PlayerSpwnRcvSystem>(ecs::Schedule::Reception);

        /**
         * DYNAMICS (Schedule::Dynamics, 30Hz)
         * Physics, movement, game logic systems.
         * Order matters - use run_after() for dependencies.
         */
        app.add_system<PlayerLifeSpwnSystem>(ecs::Schedule::Dynamics);

        // "TerrainChkSystem", NOT "TerrainChunkSystem" — the name here is the RUNTIME name a
        // system returns from name(), and terrain abbreviates: `TerrainChkSystem`. The spelled
        // out form never existed, so this edge pointed at nothing and was silently dropped: no
        // error, no warning, and the two systems fell back to registration order — which is
        // usually right, which is why it would surface as a sporadic bug rather than a broken
        // build. Both sit in Dynamics, so the edge takes effect now.
        app.add_system_with<PlayerCtrlInpSystem>(ecs::Schedule::Dynamics)
            .run_after("TerrainChkSystem");

        app.add_system_with<PlayerCtrlMovSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerCtrlInpSystem");

        /**
         * CHEAT SIMULATION (SERVER-ONLY, Phase 13 / Task 13.10 + AP-3)
         * Overrides the input-capped velocity for a backend-driven cheating player (speed-hack), so
         * PlayerSimPhysSystem applies it and PlayerAccMovSystem flags it. No-op for normal players.
         * The AP-3 archetype levers induce realised activity/communication/economy rates the same way
         * (real state on the real player entity, never a fabricated topic inject).
         */
        app.add_system_with<PlayerSimChtSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerCtrlMovSystem");

        app.add_system_with<PlayerSimActSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerCtrlMovSystem");

        app.add_system_with<PlayerSimComSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerCtrlMovSystem");

        app.add_system_with<PlayerSimEcoSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerCtrlMovSystem");

        /**
         * THE JOURNEY OF A BACKEND-DRIVEN WALKER (the phases first, the hub write after them)
         *
         * Two phases, two systems, two tags - a leg held on one heading and a pause that ends
         * with a new one. Neither touches velocity or position: PlayerHubRoamSystem lays the
         * decision on PLR_INP_FWD / PLR_CAM_YAW / PLR_INP_SPRINT and from there a walker is
         * indistinguishable from a human client, because it travels the identical seam.
         *
         * They run BEFORE PlayerCtrlMovSystem in the same tier is NOT what happens and must not
         * be arranged: the input reaches the mover through PlayerSyncInpSystem (Integration),
         * exactly as a human client's does, so a walker is one tick behind its own decision -
         * the same tick a human is behind their keyboard.
         */
        app.add_system<PlayerSimWandSystem>(ecs::Schedule::Dynamics);

        app.add_system_with<PlayerSimRestSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimWandSystem");

        app.add_system_with<PlayerHubRoamSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimRestSystem");

        app.add_system_with<PlayerSimPhysSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimChtSystem");

        app.add_system_with<PlayerStaStsSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimPhysSystem");

        app.add_system_with<PlayerStaChnkSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerStaStsSystem");

        app.add_system_with<PlayerHubPosSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerStaChnkSystem");

        /**
         * SESSION INDEX REGISTER (producer of PLR_ACTIVE_COUNT + PLR_OWNER)
         * Hub v2.0 cannot iterate, so consumers outside ase-player discover live players only
         * through these index slots. Runs after PlayerHubPosSystem so a slot a consumer resolves
         * already carries its PLR_POS_* under the same owner.
         */
        app.add_system_with<PlayerHubSessRegSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerHubPosSystem");

        /**
         * ANTI-CHEAT (SERVER-ONLY authority, Phase 13 / Task 13.10 + AP-3)
         * Reads the realised post-physics velocity and flags speed-hacks on the Hub
         * as the contract trigger "PLAYER_MOVEMENT_SUSPICIOUS" (Replica forwards to MovementValidator).
         * The AP-3 archetype detectors flag the remaining contract topics from the realised rates the
         * induction systems drive (exact-hash literals incl. `*`, ANTI_CHEAT_COMPONENT_CONTRACT.md):
         * PLAYER_ACTION_* + COMBAT_EVENT_* (BehaviorWatcher), PLAYER_COMMUNICATION_*
         * (CoordinationDetector), ECONOMY_TRANSACTION_* + INVENTORY_MODIFICATION_* (EconomyAuditor).
         */
        app.add_system_with<PlayerAccMovSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerHubPosSystem");

        app.add_system_with<PlayerAccActSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimActSystem");

        app.add_system_with<PlayerAccCmbSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimActSystem");

        app.add_system_with<PlayerAccComSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimComSystem");

        app.add_system_with<PlayerAccEcoSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimEcoSystem");

        app.add_system_with<PlayerAccInvSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimEcoSystem");

        /**
         * OBSERVATION (Schedule::Observation, 1Hz standing-watch)
         * WS-H.4 boundary-migrate serializer: answers the PlayerReqMigComponent the World
         * boundary watch (WorldPlrBndSystem, ase-world, same schedule) armed - bumps the
         * per-player monotonic migrate epoch and stages the frozen 42-byte PlayerSnap into
         * PlayerBufMigComponent. run_after orders it behind the watch so an armed migrate
         * serializes the SAME standing-watch tick; on a non-World tier the name resolves to
         * nothing and the edge is a no-op (dependency_sorter skips unknown names).
         */
        app.add_system_with<PlayerMigSerSystem>(ecs::Schedule::Observation)
            .run_after("WorldPlrBndSystem");

        /**
         * Deserializer-Haelfte der WS-H.4-Naht: verbraucht die Stern-Zustellungen
         * (HubPlrMigSnap/Ack + HubPlrMigPndTag), die der world-Empfaenger anlegt, und baut die
         * Figur mit den EIGENEN Typen. run_after ordnet ihn hinter den Zusteller, damit eine
         * Zustellung im SELBEN Takt verbraucht wird; auf Nicht-World-Tiers loest der Name auf
         * nichts auf und die Kante ist ein No-op (dependency_sorter skips unknown names).
         */
        app.add_system_with<PlayerMigDesSystem>(ecs::Schedule::Reception)
            .run_after("WorldPlrMigRcvSystem");

        /**
         * PRESERVATION (Schedule::Preservation, 1Hz)
         * Save state to MongoDB.
         */
        app.add_system<PlayerPstSerSystem>(ecs::Schedule::Preservation);

        /**
         * CONCLUSION (Schedule::Conclusion)
         * Debug/logging runs after all other schedules.
         */
        /**
         * HERZSCHLAG STATT TIMER-KRUECKE (Betreiber-Entscheid 2026-08-11): ein Zustands-Log
         * lebt im Takt seiner Aussage. In Conclusion (60 Hz Frame-Tier) tickte dieses System
         * sechzigmal je Sekunde und drosselte sich selbst per log_interval_timer - die Platte
         * ist heute an genau dieser Klasse vollgelaufen. Observation (1 Hz) IST das
         * Monitoring-Herz; der Schedule traegt die Frequenz, kein Gate, kein Timer.
         */
        app.add_system<PlayerLogObsvSystem>(ecs::Schedule::Observation);

        /**
         * INTEGRATION (Schedule::Integration)
         * Sync input state from network.
         */
        app.add_system<PlayerSyncInpSystem>(ecs::Schedule::Integration);
    }
};

}  // namespace ase::player
