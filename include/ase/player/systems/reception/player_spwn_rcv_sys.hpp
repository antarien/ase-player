#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_spwn_rcv_sys.hpp
 * @brief       PlayerSpwnRcvSystem - creates a spawn request from the Replica-forwarded backend spawn frame
 * @description Drains transport::LANE_SPW (the Replica forwards BIN_MSG_PLAYER_SPAWN here verbatim from the
 *              ase-cli frontdoor), decodes [77][player_id:char[64]][x:f32][z:f32], and creates one
 *              PlayerReqSpwnComponent request entity. PlayerLifeSpwnSystem then turns it into the real
 *              ase-player entity — a backend-driven spawn with NO human WebRTC client, induced over the
 *              correct WS transport (never a World-HTTP path). Phase 13 / Task 13.10.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    action/transfer/receive
 * @schedule    Reception
 * @created     2026-07-01
 * @modified    2026-07-01
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

class PlayerSpwnRcvSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSpwnRcvSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
