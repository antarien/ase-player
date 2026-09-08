#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_life_roam_ini_sys.hpp
 * @brief       PlayerLifeRoamIniSystem - Sets a freshly spawned walker on its errand
 * @description Reads the errand that travelled with a spawn request, caps the requested speed at
 *              the movement authority, draws the walker's first heading and first pause, and puts
 *              it into the resting phase.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    ecs/entity/entitylifecycle
 * @schedule    Dynamics
 * @created     2026-08-30
 * @modified    2026-08-30
 * @version     1.0.0
 *
 * WHY THIS SYSTEM EXISTS (split from PlayerLifeSpwnSystem, 2026-08-30)
 *
 * The origin carried 518 lines and three concerns: it created players, it set walkers on their
 * errand, and it destroyed players. The errand marked itself as a concern of its own in the
 * origin's own words - "this is the ONE place where a requested speed meets the movement
 * authority". It owns its request component, its state component, two tags, a warning and a log
 * line, and it shares none of them with the body creation next door.
 *
 * THE SEAM IS A COMPONENT, NOT A CALL. The spawn system answers every request with
 * PlayerReqSpwnResComponent on the request entity; the errand rides on that same entity. This
 * system therefore needs nothing but the pair - it never asks the spawn system anything.
 *
 * IT CONSUMES THE ERRAND, exactly as the origin did: leaving it behind would let a later pass
 * read an errand whose player already exists.
 *
 * MEASURED BEFORE THE CUT (2026-08-30): PlayerReqSpwnResComponent is written in exactly one
 * place - the spawn system - and read nowhere else in modules, plugins or servers except the
 * module test. Nothing removes the request entity between the two systems, so the pair is still
 * there when this system runs.
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
 * @brief The errand of a freshly spawned walker: speed, first heading, first pause
 *
 * @schedule Dynamics - same tier as the spawn side, ordered after it
 * @reads    PlayerReqSpwnResComponent (which entity was spawned), PlayerReqRoamComponent
 * @writes   PlayerStaRoamComponent, PlayerRoamRestTag, PlayerRoamRunTag (fast walkers only);
 *           removes PlayerReqRoamComponent
 * @depends  PlayerLifeSpwnSystem (writes the result this system reads)
 */
class PlayerLifeRoamIniSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerLifeRoamIniSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
