#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_life_spwn_sys.hpp
 * @brief       PlayerLifeSpwnSystem - Processes spawn requests and creates the player row
 * @description Creates player entities from PlayerReqSpwnComponent and answers each request with
 *              PlayerReqSpwnResComponent.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    ecs/entity/entitylifecycle
 * @schedule    Dynamics
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * SPLIT 2026-08-30: this system carried spawn, errand and despawn in one 518-line body and sat in
 * the unsplit band. Birth and death are two lifecycles; the errand named itself a third ("the ONE
 * place where a requested speed meets the movement authority"). The errand moved to
 * PlayerLifeRoamIniSystem, the despawn to PlayerLifeDespSystem; both run after this one in
 * Dynamics, the same sequence the single system had. The name stayed because what is left IS the
 * spawn.
 *
 * ARCHITECTURE:
 *
 *   Request Flow:
 *   REST Handler ──> creates request entity with PlayerReqSpwnComponent
 *   PlayerLifeSpwnSystem ──> processes request, creates player, adds result
 *   PlayerLifeRoamIniSystem ──> reads the result, sets a walker on its errand
 *   REST Handler ──> reads result from PlayerReqSpwnResComponent
 *
 * ECS SYSTEM HEADER COMPLIANCE
 *
 * [ ] STATELESS - No member variables
 * [ ] Views created on demand, not stored
 * [ ] NO direct calls to other systems
 * [ ] Communication only via Components
 * [ ] Helpers in anonymous namespace (in .cpp, NOT static functions!)
 * [ ] Math functions from ase-math (Layer 0)
 * [ ] NO file-level static/constexpr (constants → types.hpp)
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
 * @schedule Dynamics - processes lifecycle events
 * @reads    PlayerReqSpwnComponent, PlayerReqDespComponent
 * @writes   PlayerReqSpwnResComponent, creates/destroys player entities
 */
class PlayerLifeSpwnSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerLifeSpwnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
