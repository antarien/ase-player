#pragma once
/**
 * PlayerNetBctSystem - Broadcast player state via ECS message pattern
 *
 * ECS STATELESS: No private member state!
 *
 * Reads: PlayerDirtyTag, all player state components
 * Writes: Creates ReplicationMsgDatComponent + PendingBroadcast message entities
 *         Removes DirtyTag after creating messages
 *
 * Sends "player_state" messages in codegen-expected format:
 *   {playerStId: {...}, playerStPos: {...}, playerStVel: {...}, playerStSts: {...}}
 *
 * Client logic:
 *   - Entity doesn't exist? → Create it (spawn)
 *   - Entity exists? → Update it
 *
 * Uses ECS message entity pattern from ase-replication for broadcasting.
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerNetBctSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerNetBctSystem"; }

    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
