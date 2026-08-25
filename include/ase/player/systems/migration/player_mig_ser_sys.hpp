#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_mig_ser_sys.hpp
 * @brief       PlayerMigSerSystem - serialize the PlayerSnap for an armed boundary migrate
 * @description WS-H.4 serializer half (the ase-player side of the migrate seam). For every
 *              figure carrying an armed hub::HubPlrMigArmComponent (stamped by the World
 *              boundary watch over the star) and no staged delivery yet, it bumps the player's
 *              monotonic migrate epoch (PlayerStaEpchComponent - seeded from the FNV-1a32 of
 *              the UUID string on first use) and encodes the frozen 42-byte PlayerSnap layout
 *              (ase/types/region_wire.hpp) from the REAL live components -
 *              PlayerStaPosComponent (x,y,z,yaw), PlayerStaVelComponent (vx,vy,vz),
 *              PlayerStaStsComponent (status, widened u8→u32) - into the star delivery
 *              hub::HubPlrMigSnapComponent on the figure, marked hub::HubPlrMigSndPndTag. The
 *              World send system enumerates the mark, wraps the image into the frame-100 header
 *              and ships it; serialization and egress stay separate modules, and neither side
 *              reads the other's types any more (seam cut 2026-08-19).
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    entity/actor/player
 * @schedule    Observation
 * @created     2026-07-28
 * @modified    2026-07-28
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
 * @brief Encodes the frozen PlayerSnap byte image for every armed migrate instruction and
 *        bumps the per-player monotonic epoch (WS-H.4 idempotency source).
 *
 * Processes (Observation, after the World boundary watch armed the star instruction):
 *   each (hub::HubPlrMigArmComponent, PlayerStPos/Vel/Sts/Id) exclude HubPlrMigSnapComponent →
 *     seed/bump PlayerStaEpchComponent → encode PlayerSnap →
 *       stage hub::HubPlrMigSnapComponent + hub::HubPlrMigSndPndTag on the figure
 *
 * @schedule Observation
 * @reads    hub::HubPlrMigArmComponent, PlayerStaPosComponent, PlayerStaVelComponent,
 *           PlayerStaStsComponent, PlayerStaIdntComponent
 * @writes   PlayerStaEpchComponent (epoch bump), hub::HubPlrMigSnapComponent (staged delivery),
 *           hub::HubPlrMigSndPndTag (enumeration mark for the world sender)
 */
class PlayerMigSerSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerMigSerSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
