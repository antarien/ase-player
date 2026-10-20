#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_mig_des_sys.hpp
 * @brief       PlayerMigDesSystem - take the inbound migrate rows off the star into bridge components
 * @description WS-H.4 inbound half, Sync part of the SYN cut. ase-world files one hub row per
 *              inbound frame: HUB_PLR_MIG_DLV for a frozen PlayerSnap this node now owns,
 *              HUB_PLR_MIG_ACK for the echo of a figure that left. This system copies each row
 *              into a bridge entity of this module (PlayerInpMigDlvrComponent,
 *              PlayerInpMigAckComponent) and releases the row in the same visit. Building the
 *              figure and the despawn request are calculation and belong to PlayerMigAplySystem.
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    entity/actor/player
 * @schedule    Reception
 * @parity      server_only
 * @design      DSGN_021
 * @usecase     usecase:100692 - migrate figure between regions
 * @actor       Fleet Capacity Ledger (FLTCAP)
 * @created     2026-08-19
 * @modified    2026-10-14
 * @version     2.0.0
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
 * @brief Copies the inbound migrate rows of the star into bridge entities of this module.
 *
 * Processes (Reception, run_after WorldPlrMigRcvSystem):
 *   row_each HUB_PLR_MIG_DLV → bridge PlayerInpMigDlvrComponent (snap, snap_len, dst_region),
 *     row_release
 *   row_each HUB_PLR_MIG_ACK → bridge PlayerInpMigAckComponent (player_ref, ack_epoch),
 *     row_release
 *
 * @schedule Reception
 * @reads    hub rows HUB_PLR_MIG_DLV (SNAP, DST_REGION), HUB_PLR_MIG_ACK (PLAYER_REF, EPOCH)
 * @writes   PlayerInpMigDlvrComponent, PlayerInpMigAckComponent (bridge entities),
 *           row_release of the visited rows
 */
class PlayerMigDesSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerMigDesSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
