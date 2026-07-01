#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_acc_mov_sys.hpp
 * @brief       PlayerAccMovSystem - Engine-level anti-cheat movement detector
 * @description SERVER-ONLY authority system: compares a player's realised horizontal speed against
 *              the engine movement authority (types.hpp) and flags violations on the Hub as the
 *              contract trigger "PLAYER_MOVEMENT_SUSPICIOUS" (consumed by the MovementValidator skill).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state/abundance/change
 * @schedule    Dynamics
 * @created     2026-06-30
 * @modified    2026-06-30
 * @version     1.0.0
 *
 * ARCHITECTURE (SERVER-ONLY authority — never transpiled to the client):
 *
 *   PlayerStVelComponent → PlayerAccMovSystem → Hub "PLAYER_MOVEMENT_SUSPICIOUS"
 *   (realised velocity)     (vx*vx+vz*vz vs bound)  (1.0 suspicious / 0.0 clear)
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
 * @brief Engine-level anti-cheat movement detector (SERVER-ONLY)
 *
 * @schedule Dynamics — runs after PlayerHubPosSystem so it reads the realised post-physics velocity
 * @reads    PlayerStIdComponent (player identity), PlayerStVelComponent (realised velocity)
 * @writes   Hub "PLAYER_MOVEMENT_SUSPICIOUS" (per-player flag; the Replica forwards it as the MovementValidator trigger)
 */
class PlayerAccMovSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerAccMovSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
