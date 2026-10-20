#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sim_wrld_sys.hpp
 * @brief       PlayerSimWrldSystem - turns the chunk-relative place of every player figure into world metres
 * @description Every frame in Dynamics, after the place has settled and before the hub publisher: every player
 *              with a place (PlayerStaPosComponent) gets its world-metre view (PlayerStaWrldComponent) in the
 *              axis order of the star contract - the two horizontal axes as chunk times chunk edge plus local
 *              metres, the height as it stands. The publisher then only copies (SYN pattern: the calculation is
 *              pure, the hub I/O has its own system).
 *
 *              It runs after PlayerStaChnkSystem on purpose: the teleport jump and the physics step have moved
 *              the place for this frame, and the physics step has carried every overflow across the cell edge,
 *              so the view is taken from a local coordinate inside [0, edge).
 *
 *              WHY server_only: the place it reads is a server_only row of this module; the calculation itself
 *              touches no hub API, no ctx resource and no stored entity id.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @usecase     usecase:100384 - convert place to world metres
 * @actor       Scene Graph Optimizations (SGO)
 * @category    process/simulation
 * @schedule    Dynamics
 * @parity      server_only
 * @created     2026-10-12
 * @modified    2026-10-12
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   PlayerStaPosComponent → PlayerSimWrldSystem → PlayerStaWrldComponent → PlayerHubPosSystem
 *   (chunk-relative place)  (no Hub access)        (world metres, star axes)  (copies onto PLR_POS_*)
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
 * @brief Writes the world-metre view of every player's place (calc, no Hub)
 *
 * @schedule Dynamics - after PlayerStaChnkSystem, before PlayerHubPosSystem
 * @reads    PlayerStaPosComponent
 * @writes   PlayerStaWrldComponent
 */
class PlayerSimWrldSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSimWrldSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
