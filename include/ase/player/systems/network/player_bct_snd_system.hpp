#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_bct_snd_system.hpp
 * @brief       PlayerBctSndSystem - AUSGEHEND Step 3
 * @description Sends serialized player broadcasts via ReplicationMsgPndBctTag.
 *              Pure ECS pattern: reads SerialJsnFinTag, creates ReplicationMsgDatComponent.
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
 *   SerialBufJsnComponent ────┼──> PlayerBctSndSystem ──> ReplicationMsgDatComponent
 *                             │    (sends broadcast)      + ReplicationMsgPndBctTag
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
 * @brief Sends serialized player broadcasts
 *
 * @schedule Transmission - sends data over network
 * @reads    SerialJsnFinTag, SerialBufJsnComponent
 * @writes   ReplicationMsgDatComponent, ReplicationMsgPndBctTag
 */
class PlayerBctSndSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerBctSndSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
