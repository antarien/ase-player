#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_pst_ser_system.hpp
 * @brief       PlayerPstSerSystem - Player Persistence Serialization System
 * @description Serializes dirty players and delegates persistence to Replication Layer.
 *              Naming: Player + Pst(Persist) + Ser(Serialize) + System
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    persistence
 * @schedule    Preservation
 * @created     2026-01-22
 * @modified    2026-01-22
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   Pattern (Star Citizen Replication Layer):
 *   1. Looks for PlayerPstDtyTag (players needing persistence)
 *   2. Serializes player state to JSON
 *   3. Sets ReplicationPstMetComponent + ReplicationDtyTag
 *   4. Removes PlayerPstDtyTag (job done - Replication Layer takes over)
 *
 *   CRITICAL: This system does NOT write to database directly!
 *   All DB access is delegated to ase-replication via ReplicationDtyTag.
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
 * @brief Player Persistence Serialization System
 *
 * @schedule Preservation - persistence operations
 * @reads    PlayerPstDtyTag, Player state components
 * @writes   ReplicationPstMetComponent, ReplicationDtyTag, removes PlayerPstDtyTag
 */
class PlayerPstSerSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerPstSerSystem"; }
    int priority() const override { return 200; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
