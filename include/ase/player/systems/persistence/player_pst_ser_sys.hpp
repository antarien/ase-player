#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_pst_ser_sys.hpp
 * @brief       PlayerPstSerSystem - Player persistence serialization
 * @description Serializes dirty players and delegates persistence to Replication Layer.
 *              Naming: Player + Pst(Persist) + Ser(Serialize) + System
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    entity/actor/player
 * @schedule    Preservation
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     2.0.0
 *
 * ARCHITECTURE:
 *
 *   PlayerPstDtyTag + PlayerSpndTag → PlayerPstSerSystem → Hub + SerialJsnPndTag
 *   (dirty player)                       (serialize)          REP_PST_*
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
 * @brief Player persistence serialization system
 *
 * @schedule Preservation
 * @reads    PlayerPstDtyTag, PlayerSpndTag, PlayerSt*Component
 * @writes   Hub: REP_PST_SER, REP_PST_SYN, SerialBufJsnComponent, SerialJsnPndTag
 */
class PlayerPstSerSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerPstSerSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
