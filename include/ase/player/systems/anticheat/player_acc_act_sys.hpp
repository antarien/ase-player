#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_acc_act_sys.hpp
 * @brief       PlayerAccActSystem - Engine-level anti-cheat action-rate detector
 * @description SERVER-ONLY authority system (Phase 13 AP-3): compares a player's realised action rate
 *              against the engine authority (types.hpp) and flags violations on the Hub as the exact
 *              contract topic "PLAYER_ACTION_*" (consumed by the BehaviorWatcher skill; the literal
 *              `*` is part of the topic name per ANTI_CHEAT_COMPONENT_CONTRACT.md).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state/abundance/change
 * @schedule    Dynamics
 * @created     2026-07-19
 * @modified    2026-07-19
 * @version     1.0.0
 *
 * ARCHITECTURE (SERVER-ONLY authority — never transpiled to the client):
 *
 *   PlayerStaActComponent → PlayerAccActSystem → Hub "PLAYER_ACTION_*"
 *   (realised action rate)  (rate vs authority)   (1.0 suspicious / 0.0 clear)
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
 * @brief Engine-level anti-cheat action-rate detector (SERVER-ONLY)
 *
 * @schedule Dynamics — after PlayerSimActSystem so it reads the realised induced rate
 * @reads    PlayerStaIdntComponent (player identity), PlayerStaActComponent (realised action rate)
 * @writes   Hub "PLAYER_ACTION_*" (per-player flag; the Replica forwards it as the BehaviorWatcher trigger)
 */
class PlayerAccActSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerAccActSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
