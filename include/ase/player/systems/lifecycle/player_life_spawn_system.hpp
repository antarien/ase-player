#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_life_spawn_system.hpp
 * @brief       PlayerLifeSpawnSystem - Processes spawn/despawn requests
 * @description Creates/destroys player entities based on request components.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    lifecycle
 * @schedule    Integration
 * @created     2026-01-22
 * @modified    2026-01-22
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   Request Flow:
 *   REST Handler ──> creates request entity with PlayerReqSpawnComponent
 *   PlayerLifeSpawnSystem ──> processes request, creates player, adds result
 *   REST Handler ──> reads result from PlayerReqSpawnResComponent
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
 * @brief Processes spawn/despawn requests
 *
 * @schedule Integration - processes lifecycle events
 * @reads    PlayerReqSpawnComponent, PlayerReqDespawnComponent
 * @writes   PlayerReqSpawnResComponent, creates/destroys player entities
 */
class PlayerLifeSpawnSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerLifeSpawnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
