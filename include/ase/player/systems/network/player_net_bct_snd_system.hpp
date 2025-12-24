#pragma once
/**
 * PlayerNetBctSndSystem - Sends serialized player data as broadcast
 *
 * Schedule: Replication
 * Reads: SerialJsnFinTag + SerialBufJsnComponent + PlayerBct*PndTag
 * Creates: ReplicationMsgDatComponent + ReplicationMsgPndBctTag
 *
 * snd = send
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerNetBctSndSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerNetBctSndSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
