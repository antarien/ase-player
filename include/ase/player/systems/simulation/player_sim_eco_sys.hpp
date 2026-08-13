#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_eco_sys.hpp
 * @brief       PlayerSimEcoSystem - Backend cheat-induction for the economy axes (dupe flow)
 * @description SERVER-ONLY cheat simulation (Phase 13 AP-3): reads the backend lever PLR_CHEAT_DUPE
 *              (owner = hash(player_id), set via ase player cheat --dupe) and drives the player's
 *              realised transaction_rate + modification_rate above the engine authorities — a real
 *              item-duplication flow induced on the real player entity, never a fabricated topic
 *              inject. The detectors PlayerAccEcoSystem / PlayerAccInvSystem flag the contract topics
 *              from the realised rates.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    process/simulation
 * @schedule    Dynamics
 * @created     2026-07-19
 * @modified    2026-07-19
 * @version     1.0.0
 *
 * ARCHITECTURE (SERVER-ONLY cheat induction — never transpiled to the client):
 *
 *   Hub "PLR_CHEAT_DUPE" → PlayerSimEcoSystem → PlayerStaEcoComponent.transaction_rate
 *   (owner=hash(player_id))  (lever → realised)   + PlayerStaInvComponent.modification_rate
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
 * @brief Backend cheat-induction for the economy axes (SERVER-ONLY)
 *
 * @schedule Dynamics — alongside PlayerSimChtSystem, before the anti-cheat detectors
 * @reads    PlayerStaIdntComponent (player identity), Hub "PLR_CHEAT_DUPE" (owner = hash(player_id))
 * @writes   PlayerStaEcoComponent.transaction_rate, PlayerStaInvComponent.modification_rate (realised rates)
 */
class PlayerSimEcoSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimEcoSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
