#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_chnk_cnst_syn_sys.hpp
 * @brief       PlayerChnkCnstSynSystem - lays the chunk edge of the terrain protocol on the manager row
 * @description SYN split (WRFL_ASE_SYN_PATTERN): this system does the Hub READ and nothing else.
 *              It takes TER_CONST_CHUNK_SIZE from hub::GLOBAL and lays it on
 *              PlayerInpChnkCnstComponent; the player systems that turn world metres into a
 *              chunk address read only that component.
 *
 *              A CONSTANT MISSING IS NO CONSTANT: if the row is missing or not positive, the
 *              component is REMOVED rather than left standing with last tick's value.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    input
 * @parity      server_only
 * @schedule    Ingestion
 * @usecase     usecase:100384 - convert place to world metres
 * @actor       Grid Body Keeper (GRIDBODKEE)
 * @created     2026-11-01
 * @modified    2026-11-01
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
 * @brief Lays the chunk edge of the terrain protocol on the player manager row
 *
 * @reads    PlayerMgrTag (the manager row), "TER_CONST_CHUNK_SIZE"_hs (Hub, GLOBAL)
 * @writes   PlayerInpChnkCnstComponent (on the manager row; removed when the constant is missing)
 */
class PlayerChnkCnstSynSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerChnkCnstSynSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
