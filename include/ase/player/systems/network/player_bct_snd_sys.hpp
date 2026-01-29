#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_bct_snd_sys.hpp
 * @brief       PlayerBctSndSystem - Player broadcast send system
 * @description Sends serialized player broadcasts via Hub for replication.
 *              Pure ECS pattern: reads SerialJsnFinTag, writes Hub values.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    entity/actor/player
 * @schedule    Transmission
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     2.0.0
 *
 * ARCHITECTURE:
 *
 *   SerialJsnFinTag + PlayerBctSpnPndTag → PlayerBctSndSystem → Hub values
 *   (serialization complete)               (write to Hub)       REP_MSG_*
 *
 * ECS SYSTEM HEADER COMPLIANCE
 *
 * [ ] STATELESS - No member variables
 * [ ] Views created on demand, not stored
 * [ ] NO direct calls to other systems
 * [ ] Communication only via Components
 * [ ] Helpers in anonymous namespace (in .cpp, NOT static functions!)
 * [ ] Math functions from ase-math (Layer 0)
 * [ ] NO file-level static/constexpr (constants in types.hpp)
 * [ ] Registered in Module with correct Schedule
 * [ ] Filename matches convention
 * [ ] Class name derived from filename
 * [ ] ALL THREE METHODS DECLARED: on_start, tick, on_stop
 */

#include <ase/ecs/system.hpp>

namespace ase::player {

/**
 * @brief Player broadcast send system
 *
 * @schedule Transmission
 * @reads    SerialJsnFinTag, SerialBufJsnComponent, PlayerBufBctSpnComponent, PlayerBufBctStaComponent
 * @writes   Hub: REP_MSG_CHN, REP_MSG_PTR_HI, REP_MSG_PTR_LO, REP_MSG_LEN, REP_MSG_BCT
 */
class PlayerBctSndSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerBctSndSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
