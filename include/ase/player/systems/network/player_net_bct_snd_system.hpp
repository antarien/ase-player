#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_net_bct_snd_system.hpp
 * @brief       PlayerNetBctSndSystem - Sends serialized player data as broadcast
 * @description Schedule: Transmission
 *              snd = send
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    network
 * @schedule    Transmission
 * @created     2026-01-22
 * @modified    2026-01-22
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   SerialJsnFinTag ──────────┐
 *   SerialBufJsnComponent ────┼──> PlayerNetBctSndSystem ──> ReplicationMsgDatComponent
 *   PlayerBct*PndTag ─────────┘    (sends broadcast)         + ReplicationMsgPndBctTag
 *
 * ECS SYSTEM HEADER COMPLIANCE
 *
 * [ ] STATELESS - No member variables
 * [ ] Views created on demand, not stored
 * [ ] NO direct calls to other systems
 * [ ] Communication only via Components
 * [ ] Helpers in anonymous namespace (in .cpp, NOT static functions!)
 * [ ] Math functions from ase-math (Layer 0)
 * [ ] NO file-level static/constexpr (constants -> types.hpp)
 * [ ] Registered in Module with correct Schedule
 * [ ] Filename matches convention
 * [ ] Class name derived from filename
 * [ ] ALL THREE METHODS DECLARED: on_start, tick, on_stop
 */

#include <ase/ecs/system.hpp>

namespace ase::player {

/**
 * @brief Sends serialized player data as broadcast
 *
 * @schedule Transmission - sends data over network
 * @reads    SerialJsnFinTag, SerialBufJsnComponent, PlayerBct*PndTag
 * @writes   ReplicationMsgDatComponent, ReplicationMsgPndBctTag
 */
class PlayerNetBctSndSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerNetBctSndSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
