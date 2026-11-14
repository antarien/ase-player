#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_area_syn_sys.hpp
 * @brief       PlayerAreaSynSystem - lays the loaded area of its plate on every bound figure
 * @description SYN split (WRFL_ASE_SYN_PATTERN): this system does the Hub READ and nothing else. For
 *              every figure of PlayerSpndTag that carries the row HUB_PLR_AREA it reads the plate
 *              HUB_PLR_AREA_PLATE and the levelled disc of that plate (TRN_FLAT_HGT, TRN_FLAT_RAD,
 *              TRN_FLAT_X, TRN_FLAT_Z) and lays it on PlayerInpAreaComponent. PlayerSimBndySystem
 *              reads only that component.
 *
 *              AN AREA NOT MEASURED IS NO AREA: a figure without the row, without a plate or on a plate
 *              whose disc is not measured loses the component instead of keeping last pass's edge.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    input
 * @parity      server_only
 * @schedule    Ingestion
 * @usecase     usecase:101612 - keep figure in loaded area
 * @actor       Game Areal Plate (AREAL)
 * @created     2026-11-13
 * @modified    2026-11-13
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
 * @brief Lays the loaded area of its plate on every figure bound to one
 *
 * @reads    PlayerSpndTag, row HUB_PLR_AREA with HUB_PLR_AREA_PLATE (Hub, per figure),
 *           TRN_FLAT_HGT, TRN_FLAT_RAD, TRN_FLAT_X, TRN_FLAT_Z (Hub, owner = the plate)
 * @writes   PlayerInpAreaComponent (on the figure; removed when the area is not measured)
 */
class PlayerAreaSynSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerAreaSynSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
