#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_mig_ser_sys.hpp
 * @brief       PlayerMigSerSystem - serialize the PlayerSnap for an armed boundary migrate
 * @description WS-H.4 serializer half (the ase-player side of the migrate seam). For every
 *              figure carrying the hub row HUB_PLR_MIG_ARM (adopted by the World boundary
 *              watch) and no staged image yet, it bumps the player's monotonic migrate epoch
 *              (PlayerStaEpchComponent - seeded from the FNV-1a32 of the UUID string on first
 *              use) and encodes the frozen 42-byte PlayerSnap layout (ase/types/region_wire.hpp)
 *              from the live components - PlayerStaPosComponent, PlayerStaYawComponent,
 *              PlayerStaVelComponent, PlayerStaStsComponent (status, widened u8→u32) - into a
 *              new hub row HUB_PLR_MIG_STG (figure and image as fields). The World send system
 *              walks that kind, wraps the image into the frame-100 header and ships it;
 *              serialization and egress stay separate modules, and neither reads the other's types.
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    entity/actor/player
 * @schedule    Observation
 * @design      DSGN_021
 * @usecase     usecase:100692 - migrate figure between regions
 * @actor       Fleet Capacity Ledger (FLTCAP)
 * @created     2026-07-28
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
 * @brief Encodes the frozen PlayerSnap byte image for every armed migrate instruction and
 *        bumps the per-player monotonic epoch (WS-H.4 idempotency source).
 *
 * Processes (Observation, after the World boundary watch adopted the instruction row):
 *   index the figures of the standing HUB_PLR_MIG_STG rows →
 *   row_each HUB_PLR_MIG_ARM (skip an indexed figure) →
 *     seed/bump PlayerStaEpchComponent → encode PlayerSnap →
 *       row_create HUB_PLR_MIG_STG (FIG, SNAP)
 *
 * @schedule Observation
 * @reads    hub row HUB_PLR_MIG_ARM (DST_REGION, PROJ_HASH), hub row HUB_PLR_MIG_STG (FIG),
 *           PlayerStaPosComponent, PlayerStaYawComponent, PlayerStaVelComponent,
 *           PlayerStaStsComponent, PlayerStaIdntComponent
 * @writes   PlayerStaEpchComponent (epoch bump), hub row HUB_PLR_MIG_STG (staged image)
 */
class PlayerMigSerSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerMigSerSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
