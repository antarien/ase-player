#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_hub_roam_sys.hpp
 * @brief       PlayerHubRoamSystem - publishes a walker's errand as ordinary player input
 * @description SERVER-ONLY: the hub half of the journey (SYN pattern). It carries the forward
 *              magnitude and the heading of PlayerStaRoamComponent onto PLR_INP_FWD and
 *              PLR_CAM_YAW - the very keys a human client's input arrives on - and computes
 *              nothing. That is what keeps a backend-driven walker from being a second kind of
 *              actor: downstream, nobody can tell where the input came from, because there is no
 *              second path.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    hub
 * @schedule    Dynamics
 * @created     2026-08-09
 * @modified    2026-08-09
 * @version     1.0.0
 *
 * ARCHITECTURE (SYN: the two phase systems decide, this one does the I/O and nothing else):
 *
 *   PlayerStaRoamComponent → PlayerHubRoamSystem → Hub PLR_INP_FWD, PLR_CAM_YAW
 *   → PlayerSyncInpSystem → PlayerInpExtComponent → PlayerCtrlMovSystem
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
 * @brief Hub half of a backend-driven player's journey (SERVER-ONLY)
 *
 * @schedule Dynamics — after the two phase systems, so the published input is this tick's decision
 * @reads    PlayerStaRoamComponent
 * @writes   Hub PLR_INP_FWD, PLR_CAM_YAW (owner = the player entity, as the human path uses)
 */
class PlayerHubRoamSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerHubRoamSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
