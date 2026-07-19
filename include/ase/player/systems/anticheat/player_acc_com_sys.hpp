#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_acc_com_sys.hpp
 * @brief       PlayerAccComSystem - Engine-level anti-cheat communication-rate detector
 * @description SERVER-ONLY authority system (Phase 13 AP-3): compares a player's realised chat message
 *              rate against the engine authority (types.hpp) and flags violations on the Hub as the
 *              exact contract topic "PLAYER_COMMUNICATION_*" (consumed by the CoordinationDetector
 *              skill; the literal `*` is part of the topic name per ANTI_CHEAT_COMPONENT_CONTRACT.md).
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
 *   PlayerStaComComponent → PlayerAccComSystem → Hub "PLAYER_COMMUNICATION_*"
 *   (realised message rate)  (rate vs authority)  (1.0 suspicious / 0.0 clear)
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
 * @brief Engine-level anti-cheat communication-rate detector (SERVER-ONLY)
 *
 * @schedule Dynamics — after PlayerSimComSystem so it reads the realised induced rate
 * @reads    PlayerStIdComponent (player identity), PlayerStaComComponent (realised message rate)
 * @writes   Hub "PLAYER_COMMUNICATION_*" (per-player flag; the Replica forwards it as the CoordinationDetector trigger)
 */
class PlayerAccComSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerAccComSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
