#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_life_desp_sys.hpp
 * @brief       PlayerLifeDespSystem - Processes despawn requests and destroys the marked players
 * @description Answers a despawn request by marking the named player for deletion, then destroys
 *              everything carrying the pending mark. Deferred deletion: mark first, destroy after
 *              the request loop.
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
 * PlayerLifeSpwnSystem carried 518 lines and answered BOTH request kinds from one tick: it
 * created players and it destroyed them. Birth and death are two lifecycles, not two paragraphs
 * of one - they read different request components (PlayerReqSpwnComponent against
 * PlayerReqDespComponent), write different results, and share nothing but the identity index.
 *
 * ARCHITECTURE:
 *
 *   Request Flow:
 *   REST Handler ──> creates request entity with PlayerReqDespComponent
 *   PlayerLifeDespSystem ──> marks the player, destroys it, adds result
 *   REST Handler ──> reads result from PlayerReqDespResComponent
 *
 * THE IDENTITY INDEX IS BUILT HERE TOO, AND THAT IS THE POINT OF THE COMMENT
 *
 * The origin built ONE index per pass and both loops used it. After the cut each system builds
 * its own - once per tick, O(N) in players. That is NOT the defect the index was introduced
 * against on 2026-08-18: THAT one was a full view INSIDE the request loop, M requests times N
 * players, twice per tick. One build per system per tick keeps the fix; sharing the map across
 * two systems would need it in a Component, and a HashMap is not POD.
 *
 * ORDER MATTERS AND IS EXPLICIT. This system runs after PlayerLifeSpwnSystem, the same sequence
 * the single system had: a player spawned in this tick can be despawned in the same tick, and
 * not the other way round.
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
 * @brief Despawn requests and the deferred destruction that follows them
 *
 * @schedule Dynamics - same tier as the spawn side, ordered after it
 * @reads    PlayerReqDespComponent, PlayerStaIdntComponent
 * @writes   PlayerDespPndTag (mark), PlayerReqDespResComponent (answer); removes
 *           PlayerReqDespComponent; destroys the marked player entities
 * @depends  PlayerLifeSpwnSystem (a player spawned this tick must be despawnable in it)
 */
class PlayerLifeDespSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerLifeDespSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
