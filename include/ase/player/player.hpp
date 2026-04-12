#pragma once

/**
 * ASE MODULE INCLUDE HEADER (SSOT)
 *
 * @file        player.hpp
 * @brief       Single entry point for ase-player
 * @description Include this header to use the module.
 *              Exports ALL components, ALL systems, and types.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @created     2025-12-01
 * @modified    2026-01-22
 * @version     2.0.0
 *
 * DESIGN REFERENCE: DESIGN_PLAYER (Player Entity Management)
 *
 * USAGE:
 *   #include <ase/player/player.hpp>
 *   app.add_module<ase::player::PlayerModule>();
 *
 * ECS MODULE/PLUGIN INCLUDE COMPLIANCE
 *
 * [ ] types.hpp included FIRST (SSOT for constants)
 * [ ] Module/Plugin definition included ({Module}Module or {Plugin}Plugin)
 * [ ] ALL data components exported
 * [ ] ALL state components exported
 * [ ] ALL tag components exported
 * [ ] ALL input components exported (L4 plugins: Hub sync targets)
 * [ ] ALL systems exported
 * [ ] Components grouped by category with section comments
 * [ ] Systems grouped by category with section comments
 * [ ] No circular dependencies
 * [ ] No duplicate includes
 */

/**
 * TYPES AND MODULE DEFINITION (MUST BE FIRST!)
 */
#include <ase/player/types.hpp>
#include <ase/player/player_module.hpp>

/**
 * STATE COMPONENTS - Identity (1)
 */
#include <ase/player/components/state/player_st_id_component.hpp>

/**
 * STATE COMPONENTS - Position/Movement (6)
 */
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_phys_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/state/player_st_chk_component.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>

/**
 * TAG COMPONENTS - Markers (6)
 */
#include <ase/player/components/tag/player_tag_mgr_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/components/tag/player_tag_local_component.hpp>
#include <ase/player/components/tag/player_tag_init_component.hpp>
#include <ase/player/components/tag/player_tag_chunk_changed_component.hpp>

/**
 * REQUEST COMPONENTS - Spawn/Despawn (4)
 */
#include <ase/player/components/request/player_req_spawn_component.hpp>
#include <ase/player/components/request/player_req_desp_component.hpp>
#include <ase/player/components/request/player_req_spawn_res_component.hpp>
#include <ase/player/components/request/player_req_desp_res_component.hpp>

/**
 * SYSTEMS - Lifecycle (1)
 */
#include <ase/player/systems/lifecycle/player_life_spwn_sys.hpp>

/**
 * SYSTEMS - Control (2)
 */
#include <ase/player/systems/control/player_ctrl_inp_sys.hpp>
#include <ase/player/systems/control/player_ctrl_mov_sys.hpp>

/**
 * SYSTEMS - Simulation (1)
 */
#include <ase/player/systems/simulation/player_sim_phys_sys.hpp>

/**
 * SYSTEMS - State (1)
 */
#include <ase/player/systems/state/player_sta_sts_sys.hpp>

/**
 * SYSTEMS - State/Chunk (1)
 */
#include <ase/player/systems/state/player_sta_chnk_sys.hpp>

/**
 * SYSTEMS - Hub (1)
 */
#include <ase/player/systems/hub/player_hub_pos_sys.hpp>
