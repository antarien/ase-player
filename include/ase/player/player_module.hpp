#pragma once

/**
 * ASE MODULE DEFINITION
 *
 * @file        player_module.hpp
 * @brief       Module registration for ase-player
 * @description Registers all systems with the App scheduler.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @design      DSGN_021
 * @design      DSGN_030
 * @usecase     usecase:5130 - play a character
 * @created     2025-12-01
 * @modified    2026-11-06
 * @version     00.01.42.00092 [feat]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
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
#include <ase/player/systems/lifecycle/player_life_roam_ini_sys.hpp>
#include <ase/player/systems/lifecycle/player_life_desp_sys.hpp>
#include <ase/player/systems/control/player_ctrl_inp_sys.hpp>
#include <ase/player/systems/control/player_ctrl_mov_sys.hpp>
#include <ase/player/systems/control/player_ctrl_turn_sys.hpp>
#include <ase/player/systems/simulation/player_sim_stnd_sys.hpp>
#include <ase/player/systems/simulation/player_sim_walk_sys.hpp>
#include <ase/player/systems/state/player_sta_sts_sys.hpp>
#include <ase/player/systems/state/player_sta_gait_sys.hpp>
#include <ase/player/systems/state/player_sta_chnk_sys.hpp>
#include <ase/player/systems/hub/player_hub_pos_sys.hpp>
#include <ase/player/systems/hub/player_hub_roam_sys.hpp>
#include <ase/player/systems/hub/player_hub_sess_reg_sys.hpp>
#include <ase/player/systems/hub/player_hub_sess_strt_sys.hpp>
#include <ase/player/systems/hub/player_hub_spnd_sys.hpp>
#include <ase/player/systems/hub/player_hub_spwn_sys.hpp>
#include <ase/player/systems/hub/player_hub_turn_sys.hpp>
#include <ase/player/systems/hub/player_hub_motn_sys.hpp>
#include <ase/player/systems/hub/player_hub_walk_sys.hpp>
#include <ase/player/systems/persistence/player_pst_ser_sys.hpp>
#include <ase/player/systems/log/player_log_obsv_sys.hpp>
#include <ase/player/systems/sync/player_sync_inp_sys.hpp>
#include <ase/player/systems/sync/player_sync_tlpt_sys.hpp>
#include <ase/player/systems/sync/player_chnk_cnst_syn_sys.hpp>
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
#include <ase/player/systems/simulation/player_sim_tlpt_sys.hpp>
#include <ase/player/systems/simulation/player_sim_wrld_sys.hpp>
#include <ase/player/systems/simulation/player_sim_knbk_sys.hpp>
#include <ase/player/systems/simulation/player_sim_bndy_sys.hpp>
#include <ase/player/systems/sync/player_cmb_syn_sys.hpp>
#include <ase/player/systems/sync/player_area_syn_sys.hpp>
#include <ase/player/systems/sync/player_hub_stnd_sync_sys.hpp>
#include <ase/player/systems/sync/player_hub_watr_sync_sys.hpp>
#include <ase/player/systems/reception/player_spwn_rcv_sys.hpp>
#include <ase/player/systems/migration/player_mig_aply_sys.hpp>
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
         * The same request for the entry of an embedding host: every row HUB_PLR_SPWN_REQ of
         * ase-pl-avatar becomes a PlayerReqSpwnComponent (usecase:101557 - receive a spawn request).
         */
        app.add_system<PlayerHubSpwnSystem>(ecs::Schedule::Reception);
        /**
         * The turn on the spot of an embedding host: every row HUB_PLR_TURN_REQ of ase-pl-avatar becomes a
         * PlayerInpTurnComponent on a request entity of its own, taken by PlayerCtrlTurnSystem in Dynamics
         * of the same tick (usecase:101554 - receive player input, fig.turn 07.7).
         */
        app.add_system<PlayerHubTurnSystem>(ecs::Schedule::Reception);
        /**
         * The answer of ase-physics (row HUB_PHYS_STAND, mark HUB_PHYS_STAND_GRND) onto every walking
         * figure; PlayerSimStndSystem reads it in Dynamics without touching the Hub (07.13).
         */
        app.add_system<PlayerHubStndSyncSystem>(ecs::Schedule::Reception);
        // The water depth ase-gis files under every figure (GIS_HYD_CELL_DEPTH; CAUSA_ASE_HYDRO, "Wasser je
        // Ort", item 6; B0358): PlayerStaStsSystem lays PlayerStndWatrTag from it in Dynamics.
        app.add_system<PlayerHubWatrSyncSystem>(ecs::Schedule::Reception);

        /**
         * INGESTION (Schedule::Ingestion)
         * The chunk edge of the terrain protocol (TER_CONST_CHUNK_SIZE) onto the manager row, read
         * by every player system that turns world metres into a chunk address and back.
         */
        app.add_system<PlayerChnkCnstSynSystem>(ecs::Schedule::Ingestion);

        /**
         * The movement restriction of the combat star (slow, frost, landing stun, blunder recovery,
         * knockback) onto the bridge rows of every figure; PlayerCtrlMovSystem and
         * PlayerSimKnbkSystem read them in Dynamics without touching the Hub.
         */
        app.add_system<PlayerCmbSynSystem>(ecs::Schedule::Ingestion);

        /**
         * The loaded area of its plate (row HUB_PLR_AREA, disc TRN_FLAT_*) onto every bound figure;
         * PlayerSimBndySystem reads it in Dynamics without touching the Hub (07.7).
         */
        app.add_system<PlayerAreaSynSystem>(ecs::Schedule::Ingestion);

        /**
         * DYNAMICS (Schedule::Dynamics, 30Hz)
         * Physics, movement, game logic systems.
         * Order matters - use run_after() for dependencies.
         */
        /**
         * LEBENSZYKLUS-KETTE (Schnitt 2026-08-30): Geburt → Auftrag → Tod.
         * PlayerLifeSpwnSystem trug alle drei in einem tick und lag im Bandbefund. Die zwei
         * Kanten sichern eine LESEreihenfolge, keine Nachbarschaft: PlayerLifeRoamIniSystem
         * liest das PlayerReqSpwnResComponent, das die Geburt schreibt, und PlayerLifeDespSystem
         * muss einen im selben Takt geborenen Spieler noch finden koennen. Alle drei stehen in
         * Dynamics — run_after loest nur INNERHALB einer Schedule auf.
         */
        app.add_system<PlayerLifeSpwnSystem>(ecs::Schedule::Dynamics);
        app.add_system_with<PlayerLifeRoamIniSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerLifeSpwnSystem");
        app.add_system_with<PlayerLifeDespSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerLifeRoamIniSystem");

        /**
         * DIE FIGUR IM STERN. PlayerHubSpndSystem traegt jede PlayerSpndTag-Figur als Zeile
         * HUB_PLR_SPND in den Stern. Die Kante liest, was die Geburt in DIESEM Takt gesetzt hat;
         * eine zugewanderte Figur kommt in Reception und steht damit schon davor.
         */
        app.add_system_with<PlayerHubSpndSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerLifeSpwnSystem");

        // "TerrainChkSystem", NOT "TerrainChunkSystem" — the name here is the RUNTIME name a
        // system returns from name(), and terrain abbreviates: `TerrainChkSystem`. The spelled
        // out form never existed, so this edge pointed at nothing and was silently dropped: no
        // error, no warning, and the two systems fell back to registration order — which is
        // usually right, which is why it would surface as a sporadic bug rather than a broken
        // build. Both sit in Dynamics, so the edge takes effect now.
        app.add_system_with<PlayerCtrlInpSystem>(ecs::Schedule::Dynamics)
            .run_after("TerrainChkSystem");

        /**
         * THE TURN ON THE SPOT (fig.turn 07.7). PlayerCtrlTurnSystem turns a standing figure out of the
         * open course its host filed. The edge puts the walking input of this tick before it: a figure
         * that walks keeps the heading PlayerCtrlInpSystem gave it, and its open course is dropped.
         */
        app.add_system_with<PlayerCtrlTurnSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerCtrlInpSystem");

        /**
         * THE PLACE ase-physics WALKED THE FIGURE TO (07.13). Gravity, ground and falling are values of
         * ase-physics; PlayerSimStndSystem takes its answer back into the chunk address before the
         * steering reads the ground contact. The edge to the birth lets a figure born in this pass carry
         * no answer yet instead of a stale one.
         */
        app.add_system_with<PlayerSimStndSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerLifeSpwnSystem");

        app.add_system_with<PlayerCtrlMovSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerCtrlInpSystem")
            .run_after("PlayerSimStndSystem");

        /**
         * CHEAT SIMULATION (SERVER-ONLY, Phase 13 / Task 13.10 + AP-3)
         * Overrides the input-capped velocity for a backend-driven cheating player (speed-hack), so
         * PlayerSimWalkSystem files it to ase-physics and PlayerAccMovSystem flags it. No-op for normal players.
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

        // The teleport lever (register D0703) moves the POSITION, not the velocity, and must run
        // before PlayerSimBndySystem and PlayerSimWalkSystem: the jump is a new placement, filed to
        // ase-physics in the same tick, and PlayerStaChnkSystem carries it across the cell edge.
        app.add_system_with<PlayerSimTlptSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerCtrlMovSystem");

        // The knockback of the combat star moves the POSITION by the displacement of this frame and
        // runs before PlayerSimBndySystem and PlayerSimWalkSystem for the same reason as the teleport
        // lever: the displaced place is a new placement filed in the same tick.
        app.add_system_with<PlayerSimKnbkSystem>(ecs::Schedule::Dynamics)
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

        // The area keeps the figure inside its loaded disc (07.7). It runs after every system that sets
        // place or velocity in this frame and before every reader of them: status, gait, chunk stamp and
        // the filed step see the bounded values of this frame.
        app.add_system_with<PlayerSimBndySystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimChtSystem")
            .run_after("PlayerSimTlptSystem")
            .run_after("PlayerSimKnbkSystem");

        /**
         * THE STEP FILED TO ase-physics (07.13, SYN pattern). PlayerSimWalkSystem lays frame and placement
         * and files the wish; PlayerHubWalkSystem copies it onto HUB_PHYS_WALK and computes nothing. Both
         * edges are read orders.
         */
        app.add_system_with<PlayerSimWalkSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimBndySystem");

        app.add_system_with<PlayerHubWalkSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimWalkSystem");

        app.add_system_with<PlayerStaStsSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimStndSystem")
            .run_after("PlayerSimBndySystem");

        // The gait tags read the velocity PlayerSimStndSystem took from ase-physics and
        // PlayerSimBndySystem bounded in this frame (07.5, 07.7, 07.13).
        app.add_system_with<PlayerStaGaitSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimStndSystem")
            .run_after("PlayerSimBndySystem");

        // The motion row of the star (07.8): course, horizontal velocity and the gait mark of this
        // frame, read by the drawing host. Both edges are read orders: the gait tag is settled, and
        // a figure born in this pass already stands on the roster.
        app.add_system_with<PlayerHubMotnSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerStaGaitSystem")
            .run_after("PlayerHubSpndSystem");

        app.add_system_with<PlayerStaChnkSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerStaStsSystem")
            .run_after("PlayerSimBndySystem")
            .run_after("PlayerSimWalkSystem");

        /**
         * THE WORLD-METRE VIEW, THEN ITS PUBLICATION (SYN pattern)
         * PlayerSimWrldSystem turns the settled chunk-relative place into world metres in the axis
         * order of the star; PlayerHubPosSystem copies that view onto PLR_POS_* and computes nothing.
         * The edge is a read order: the publisher reads the row the conversion writes in this frame.
         */
        app.add_system_with<PlayerSimWrldSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerStaChnkSystem");

        app.add_system_with<PlayerHubPosSystem>(ecs::Schedule::Dynamics)
            .run_after("PlayerSimWrldSystem");

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
         * WS-H.4 boundary-migrate serializer: answers the hub row HUB_PLR_MIG_ARM the World
         * boundary watch (WorldPlrBndSystem, ase-world, same schedule) adopted onto the figure -
         * bumps the per-player monotonic migrate epoch and files the frozen 42-byte PlayerSnap
         * as the hub row HUB_PLR_MIG_STG. run_after orders it behind the watch so an armed
         * migrate serializes the SAME standing-watch tick; on a non-World tier the name resolves
         * to nothing and the edge is a no-op (dependency_sorter skips unknown names).
         */
        app.add_system_with<PlayerMigSerSystem>(ecs::Schedule::Observation)
            .run_after("WorldPlrBndSystem");

        /**
         * Inbound half of the WS-H.4 seam, cut by the SYN pattern. PlayerMigDesSystem copies the
         * rows HUB_PLR_MIG_DLV and HUB_PLR_MIG_ACK the world receiver files into bridge entities
         * and releases the rows; run_after puts it behind the receiver so a delivery is taken the
         * SAME tick (on a non-World tier the name resolves to nothing and the edge is a no-op).
         * PlayerMigAplySystem builds the figure from the bridges with the module's OWN types and
         * runs right after the copy.
         */
        app.add_system_with<PlayerMigDesSystem>(ecs::Schedule::Reception)
            .run_after("WorldPlrMigRcvSystem");
        app.add_system_with<PlayerMigAplySystem>(ecs::Schedule::Reception)
            .run_after("PlayerMigDesSystem");

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
         * SESSION START (register D0703): the producer of PLAYER_SESSION_START, the subscription
         * of CoordinationDetector. Observation runs after Dissemination in the same 1 Hz tier, so
         * the flag goes up in one pass and down in the next, each edge in its own dissemination —
         * an event, never a held state the Replica would re-fire every heartbeat.
         */
        app.add_system<PlayerHubSessStrtSystem>(ecs::Schedule::Observation);

        /**
         * INTEGRATION (Schedule::Integration)
         * Sync input state from network.
         */
        app.add_system<PlayerSyncInpSystem>(ecs::Schedule::Integration);
        // The bridge of the teleport lever (D0703): Hub read here, the jump in PlayerSimTlptSystem.
        app.add_system<PlayerSyncTlptSystem>(ecs::Schedule::Integration);
    }
};

}  // namespace ase::player
