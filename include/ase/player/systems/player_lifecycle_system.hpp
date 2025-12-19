#pragma once

/**
 * PlayerLifecycleSystem
 *
 * Processes spawn/despawn requests.
 * Creates/destroys player entities based on request components.
 *
 * Request Flow:
 *   REST Handler → creates request entity with PlayerSpawnRequestComponent
 *   PlayerLifecycleSystem → processes request, creates player, adds result
 *   REST Handler → reads result from PlayerSpawnResultComponent
 */

#include <ase/ecs/ecs.hpp>

namespace ase::player {

class PlayerLifecycleSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerLifecycleSystem"; }

    void on_start(ecs::Registry& registry) override;
    void on_stop(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
};

}  // namespace ase::player
