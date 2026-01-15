#pragma once

/**
 * PlayerPstSerSystem - Player Persistence Serialization System
 *
 * Serializes dirty players and delegates persistence to Replication Layer.
 *
 * Pattern (Star Citizen Replication Layer):
 * 1. Looks for PlayerPstDtyTag (players needing persistence)
 * 2. Serializes player state to JSON
 * 3. Sets ReplicationPstMetComponent + ReplicationDtyTag
 * 4. Removes PlayerPstDtyTag (job done - Replication Layer takes over)
 *
 * CRITICAL: This system does NOT write to database directly!
 * All DB access is delegated to ase-replication via ReplicationDtyTag.
 *
 * Naming: Player + Pst(Persist) + Ser(Serialize) + System
 */

#include <ase/ecs/system.hpp>

namespace ase::player {

class PlayerPstSerSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerPstSerSystem"; }
    int priority() const override { return 200; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
