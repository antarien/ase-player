#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_cht_sys.hpp
 * @brief       PlayerSimChtSystem - Backend cheat-simulation driver (speed-hack)
 * @description SERVER-ONLY: drives a real ase-player entity to cheat (Phase 13 / Task 13.10). Reads the
 *              backend cheat request from the Hub ("PLR_CHEAT_SPEED", set by POST /api/player/cheat) and
 *              overrides the player's velocity to a superhuman horizontal speed above the engine movement
 *              authority. Simulates a hacked client ON the real player entity — no fabricated trigger, no NPC.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state/abundance/change
 * @schedule    Dynamics
 * @created     2026-06-30
 * @modified    2026-06-30
 * @version     1.0.0
 *
 * ARCHITECTURE (SERVER-ONLY cheat simulation, runs after PlayerCtrlMovSystem caps legit velocity):
 *
 *   Hub "PLR_CHEAT_SPEED" → PlayerSimChtSystem → PlayerStVelComponent (forced) + PlayerStaChtComponent
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
 * @brief Backend cheat-simulation driver (SERVER-ONLY)
 *
 * @schedule Dynamics — runs after PlayerCtrlMovSystem so it overrides the input-capped velocity
 * @reads    PlayerStIdComponent (player_id → Hub owner), Hub "PLR_CHEAT_SPEED"
 * @writes   PlayerStVelComponent (forced superhuman velocity), PlayerStaChtComponent (cheat-state marker)
 */
class PlayerSimChtSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimChtSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
