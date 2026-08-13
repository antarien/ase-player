#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_sess_reg_sys.hpp
 * @brief       PlayerHubSessRegSystem - Publish the live session index register on the Hub
 * @description Hub v2.0 has no iteration API, so a consumer cannot ask "which players are live?".
 *              The contract is an index register: PLR_ACTIVE_COUNT (GLOBAL) says how many slots
 *              are live, and PLR_OWNER read at the owner hash of "PLR_ACTIVE_<i>" yields the
 *              player entity id of slot i. ase-player owns the player entities, so it owns the
 *              register. Consumers reach it without ever including ase-player.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    hub
 * @schedule    Dynamics
 * @created     2026-08-06
 * @modified    2026-08-06
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   PlayerStaIdntComponent  ──> PlayerHubSessRegSystem ──> Hub
 *   PlayerStaPosComponent     (writes the register)      PLR_ACTIVE_COUNT (GLOBAL)
 *   (the live players)                                  PLR_OWNER (owner = hash "PLR_ACTIVE_<i>")
 *
 *   Consumers, purely over Hub values, no ase-player dependency:
 *     TerrainStrmObsSyncSystem (ase-terrain), GisColSessSynSystem (ase-gis),
 *     world_player_routes (ase-pl-webserver), predator behaviour logging (ase-pl-predator)
 *
 * WHY IT EXISTS (measured 2026-08-06):
 *   Both keys were declared in modules/ase-hub/data/hub_metrics.json but had NO producer in
 *   production code. The only hub::set calls repo-wide were the terrain test fixture
 *   (modules/ase-terrain/tests/test_terrain_sess.cpp:69,76), which writes the register itself
 *   and therefore passed while the live system was dead. A declared key without a producer reads
 *   as 0.0 and never as NOT_FOUND, so TerrainStrmObsSyncSystem returned at its count gate before
 *   its first pass: no observer, no chunk request, no version bump, no cell rows.
 *
 * SELF-HEALING:
 *   The register is recomputed from the live view on EVERY tick, carrying no state across ticks.
 *   Crash, restart or tier start order cannot leave it stale: a despawned player lowers the count
 *   on the next tick, and consumers only walk slots below the count.
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
 * @brief Publish the live session index register (PLR_ACTIVE_COUNT + PLR_OWNER per slot)
 *
 * @schedule Dynamics - after PlayerHubPosSystem, so a slot a consumer resolves already carries
 *           its PLR_POS_* under the same owner
 * @reads    PlayerStaIdntComponent, PlayerStaPosComponent
 * @writes   Hub values: PLR_ACTIVE_COUNT (GLOBAL), PLR_OWNER (owner = hash of "PLR_ACTIVE_<i>")
 */
class PlayerHubSessRegSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubSessRegSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
