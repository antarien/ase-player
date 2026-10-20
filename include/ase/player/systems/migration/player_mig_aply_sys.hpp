#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_mig_aply_sys.hpp
 * @brief       PlayerMigAplySystem - build migrated-in figures and honour migrate echoes
 * @description WS-H.4 inbound half, Calc part of the SYN cut. Reads the bridge entities
 *              PlayerMigDesSystem filled the same tick: a delivery builds or updates the figure
 *              epoch-idempotently from the frozen PlayerSnap with this module's own types, an
 *              echo files the module-own despawn request for the source copy. Every bridge entity
 *              is destroyed after the walks, refused ones included. Server only: the inputs are
 *              the bridge components of a hand-over between two World nodes, which a client never
 *              receives.
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    entity/actor/player
 * @schedule    Reception
 * @parity      server_only
 * @design      DSGN_021
 * @usecase     usecase:100692 - migrate figure between regions
 * @actor       Fleet Capacity Ledger (FLTCAP)
 * @created     2026-10-14
 * @modified    2026-10-14
 * @version     1.0.0
 *
 * ECS SYSTEM HEADER COMPLIANCE
 *
 * [ ] STATELESS - No member variables
 * [ ] Views created on demand, not stored
 * [ ] NO direct calls to other systems
 * [ ] Communication only via Components
 * [ ] Helpers in anonymous namespace (in .cpp, NOT static functions!)
 * [ ] Math functions from ase-math (Layer 0)
 * [ ] NO file-level static/constexpr (constants → types.hpp)
 * [ ] Registered in Module with correct Schedule
 * [ ] Filename matches convention
 * [ ] Class name derived from filename
 * [ ] ALL THREE METHODS DECLARED: on_start, tick, on_stop
 */

#include <ase/ecs/system.hpp>

namespace ase::player {

/**
 * @brief Applies the inbound migrate bridges: figure from a delivery, despawn request from an echo.
 *
 * Processes (Reception, run_after PlayerMigDesSystem):
 *   pre-pass: O(1) index player_ref → figure
 *   each PlayerInpMigDlvrComponent → check snap_len + schema → epoch-idempotent
 *     instantiate/update (PlayerSta*, PlayerSpndTag)
 *   each PlayerInpMigAckComponent → PlayerReqDespComponent from the OWN identity row
 *   destroy every bridge entity in one batch
 *
 * @schedule Reception
 * @reads    PlayerInpMigDlvrComponent, PlayerInpMigAckComponent, PlayerStaEpchComponent (lookup),
 *           PlayerStaIdntComponent (despawn identity)
 * @writes   PlayerSta* rows of the instantiated figure, PlayerSpndTag, PlayerReqDespComponent,
 *           destroys the bridge entities
 */
class PlayerMigAplySystem : public ecs::System {
public:
    const char* name() const override { return "PlayerMigAplySystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
