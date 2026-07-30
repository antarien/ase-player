#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sync_inp_sys.hpp
 * @brief       PlayerSyncInpSystem - Sync Hub input values to Input Component
 * @description SYN PATTERN: Reads Hub values and writes to PlayerInpExtComponent.
 *              Calculation systems read from PlayerInpExtComponent (no Hub access).
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    input
 * @schedule    Synchronization
 * @created     2026-01-22
 * @modified    2026-01-29
 * @version     1.1.0
 *
 * ARCHITECTURE:
 *
 *   Hub Values ──> PlayerSyncInpSystem ──> PlayerInpExtComponent
 *   PLR_INP_FWD     (reads Hub)             (bridge data)
 *   PLR_INP_STR
 *   PLR_CAM_YAW
 *   TRN_HGT_AT_POS
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
 * @brief Sync Hub input values to PlayerInpExtComponent
 *
 * @schedule Synchronization - before calculation systems
 * @reads    Hub values: PLR_INP_FWD, PLR_INP_STR, PLR_CAM_YAW, TRN_HGT_AT_POS
 * @writes   PlayerInpExtComponent
 */
class PlayerSyncInpSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSyncInpSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
