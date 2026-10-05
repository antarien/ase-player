#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_sync_tlpt_sys.hpp
 * @brief       PlayerSyncTlptSystem - Sync the backend teleport lever into its bridge component
 * @description SYN PATTERN: reads the Hub lever PLR_CHEAT_TELEPORT (owner = hash(player_id), set
 *              via ase player cheat --teleport) into PlayerInpTlptComponent; PlayerSimTlptSystem
 *              applies the jump from there without any Hub access. Phase 13, register D0703.
 *              It also republishes the lever's project label onto the player ENTITY owner, so the
 *              trigger the detector raises is attributed to the customer project — Hub I/O, so it
 *              belongs here and not to the calc system.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    input
 * @schedule    Integration
 * @created     2026-10-03
 * @modified    2026-10-03
 * @version     1.0.0
 *
 * ARCHITECTURE:
 *
 *   Hub Value ────────────> PlayerSyncTlptSystem ──> PlayerInpTlptComponent
 *   PLR_CHEAT_TELEPORT       (reads Hub)              (bridge data, jump_m)
 *   PLR_PROJECT_HASH_HI/LO ─> republished onto the entity owner while a jump is set
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
 * @brief Syncs the backend teleport lever into PlayerInpTlptComponent (SERVER-ONLY)
 *
 * @schedule Integration — the same tier as PlayerSyncInpSystem, before Dynamics
 * @reads    PlayerStaIdntComponent (player identity), Hub "PLR_CHEAT_TELEPORT",
 *           "PLR_PROJECT_HASH_HI/LO" (owner = hash(player_id))
 * @writes   PlayerInpTlptComponent.jump_m, Hub "PLR_PROJECT_HASH_HI/LO" (owner = player entity)
 */
class PlayerSyncTlptSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSyncTlptSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
