#pragma once
/**
 * PlayerLifeSpawnSystem
 *
 * Processes spawn/despawn requests.
 * Creates/destroys player entities based on request components.
 *
 * Request Flow:
 *   REST Handler → creates request entity with PlayerReqSpawnComponent
 *   PlayerLifeSpawnSystem → processes request, creates player, adds result
 *   REST Handler → reads result from PlayerReqSpawnResComponent
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerLifeSpawnSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerLifeSpawnSystem"; }

    void on_start(ecs::Registry& registry) override;
    void on_stop(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
};

}  // namespace ase::player
