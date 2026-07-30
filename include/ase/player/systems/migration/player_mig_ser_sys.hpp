#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_mig_ser_sys.hpp
 * @brief       PlayerMigSerSystem - serialize the PlayerSnap for an armed boundary migrate
 * @description WS-H.4 serializer half (the ase-player side of the migrate seam). For every
 *              player carrying an armed PlayerReqMigComponent (stamped by the World boundary
 *              watch) and no staged buffer yet, it bumps the player's monotonic migrate epoch
 *              (PlayerStaEpchComponent - seeded from the FNV-1a32 of the UUID string on first
 *              use) and encodes the frozen 42-byte PlayerSnap layout (ase/types/region_wire.hpp)
 *              from the REAL live components - PlayerStPosComponent (x,y,z,yaw),
 *              PlayerStVelComponent (vx,vy,vz), PlayerStStsComponent (status, widened u8→u32) -
 *              into PlayerBufMigComponent. The World send system wraps the buffer into the
 *              frame-100 header and ships it; serialization and egress stay separate modules.
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
 * @brief Encodes the frozen PlayerSnap byte image for every armed migrate request and bumps
 *        the per-player monotonic epoch (WS-H.4 idempotency source).
 *
 * Processes (Observation, after the World boundary watch armed the request):
 *   each (PlayerReqMigComponent, PlayerStPos/Vel/Sts/Id) exclude PlayerBufMigComponent →
 *     seed/bump PlayerStaEpchComponent → encode PlayerSnap → stage PlayerBufMigComponent
 *
 * @schedule Observation
 * @reads    PlayerReqMigComponent, PlayerStPosComponent, PlayerStVelComponent,
 *           PlayerStStsComponent, PlayerStIdComponent
 * @writes   PlayerStaEpchComponent (epoch bump), PlayerBufMigComponent (staged snap bytes)
 */
class PlayerMigSerSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerMigSerSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
