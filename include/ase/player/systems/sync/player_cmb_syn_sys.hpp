#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_cmb_syn_sys.hpp
 * @brief       PlayerCmbSynSystem - lays the movement restriction of the combat star on each player figure
 * @description SYN split (WRFL_ASE_SYN_PATTERN): this system does the Hub READ and nothing else.
 *              It takes slow, frost, landing stun, blunder stun and knockback of every figure from
 *              the combat star and lays them on PlayerInpCmbComponent and PlayerInpCmbKnbkComponent;
 *              the mover and the knockback step read only those components.
 *
 *              NOT AFFLICTED IS THE NORMAL STATE: a key the star does not carry for the figure
 *              writes 0 without a log line. A slow or frost value outside [0, 1] is reported and
 *              writes 0. The knockback row stands only while a knockback runs.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    input
 * @parity      server_only
 * @schedule    Ingestion
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-06
 * @modified    2026-11-06
 * @version     1.0.0
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
 * @brief Lays the movement restriction of the combat star on each player figure
 *
 * @reads    PlayerStaPosComponent, PlayerStaYawComponent (select the figures PlayerSyncInpSystem
 *           gives a movement input, spawned and migrated alike); "CMB_SLW_ACTIVE"_hs, "CMB_SLW_INTENSITY"_hs,
 *           "CMB_FRZ_STA_SLOW_FACTOR"_hs, "CMB_LANDING_STUN"_hs, "CMB_PATZER_STUN"_hs,
 *           "CMB_KBK_ACTIVE"_hs, "CMB_KBK_DISP_X"_hs, "CMB_KBK_DISP_Y"_hs, "CMB_KBK_DISP_Z"_hs
 *           (Hub, per figure)
 * @writes   PlayerInpCmbComponent; PlayerInpCmbKnbkComponent (removed without a knockback)
 */
class PlayerCmbSynSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerCmbSynSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
