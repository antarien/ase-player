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
#include <ase/player/components/state/player_sta_idnt_comp.hpp>

/**
 * STATE COMPONENTS - Position/Movement (6)
 */
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_yaw_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_phys_comp.hpp>
#include <ase/player/components/state/player_sta_sts_comp.hpp>
#include <ase/player/components/state/player_sta_chk_comp.hpp>
#include <ase/player/components/state/player_sta_roam_comp.hpp>

/**
 * TAG COMPONENTS - Markers (9)
 */
#include <ase/player/components/tag/player_roam_wand_tag.hpp>
#include <ase/player/components/tag/player_roam_rest_tag.hpp>
#include <ase/player/components/tag/player_roam_run_tag.hpp>
#include <ase/player/components/tag/player_mgr_tag.hpp>
#include <ase/player/components/tag/player_drty_tag.hpp>
#include <ase/player/components/tag/player_spnd_tag.hpp>
#include <ase/player/components/tag/player_locl_tag.hpp>
#include <ase/player/components/tag/player_init_tag.hpp>
#include <ase/player/components/tag/player_chnk_chgd_tag.hpp>

/**
 * REQUEST COMPONENTS - Spawn/Despawn/Errand (5)
 */
#include <ase/player/components/request/player_req_roam_comp.hpp>
#include <ase/player/components/request/player_req_spwn_comp.hpp>
#include <ase/player/components/request/player_req_desp_comp.hpp>
#include <ase/player/components/request/player_req_spwn_res_comp.hpp>
#include <ase/player/components/request/player_req_desp_res_comp.hpp>

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
 * SYSTEMS - Simulation (3)
 */
#include <ase/player/systems/simulation/player_sim_phys_sys.hpp>
#include <ase/player/systems/simulation/player_sim_wand_sys.hpp>
#include <ase/player/systems/simulation/player_sim_rest_sys.hpp>

/**
 * SYSTEMS - State (1)
 */
#include <ase/player/systems/state/player_sta_sts_sys.hpp>

/**
 * SYSTEMS - State/Chunk (1)
 */
#include <ase/player/systems/state/player_sta_chnk_sys.hpp>

/**
 * SYSTEMS - Hub (2)
 */
#include <ase/player/systems/hub/player_hub_pos_sys.hpp>
#include <ase/player/systems/hub/player_hub_roam_sys.hpp>
