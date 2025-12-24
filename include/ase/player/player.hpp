#pragma once
/**
 * ASE Player Module
 *
 * Provides player entity management using focused ECS components.
 *
 * Components (state/):
 *   - PlayerStIdComponent: Player ID and session timestamps
 *   - PlayerStPosComponent: World position and rotation
 *   - PlayerStVelComponent: Current velocity vector
 *   - PlayerStPhysComponent: Ground contact and physics flags
 *   - PlayerStStsComponent: State machine
 *   - PlayerStChkComponent: Current chunk position
 *   - PlayerStMovComponent: Movement settings
 *
 * Components (tag/):
 *   - PlayerMgrTag: Manager entity marker (holds config)
 *   - PlayerDirtyTag, PlayerSpawnedTag, PlayerLocalTag, etc.
 *
 * Components (request/):
 *   - PlayerReqSpawnComponent: Request to spawn a player
 *   - PlayerReqDespComponent: Request to despawn a player
 *   - PlayerReqSpawnResComponent: Result of spawn request
 *   - PlayerReqDespResComponent: Result of despawn request
 *
 * Systems:
 *   - PlayerLifeSpawnSystem: Process spawn/despawn requests
 *   - PlayerCtrlInputSystem: Process WebRTC input
 *   - PlayerCtrlMoveSystem: Calculate velocity from input
 *   - PlayerSimPhysSystem: Apply gravity, terrain collision
 *   - PlayerStateStatusSystem: Update state machine
 *   - PlayerSpatialChunkSystem: Detect chunk changes
 *   - PlayerNetBctReqSystem: Create serialization requests (FixedUpdate)
 *   - PlayerNetBctSndSystem: Send broadcasts from serialized data (Replication)
 *
 * Dependencies:
 *   - ase-input: InputComponent for buffered client input
 *   - ase-camera: CameraComponent for camera state
 *   - ase-math: Vec3 for vector operations
 *   - ase-ecs: ECS registry and system base
 *   - ase-terrain: Terrain height queries
 */

// Types
#include <ase/player/types.hpp>

// Components - State (runtime state)
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_phys_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/state/player_st_chk_component.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>

// Components - Tags (empty structs for ECS filtering)
#include <ase/player/components/tag/player_tag_mgr_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/components/tag/player_tag_local_component.hpp>
#include <ase/player/components/tag/player_tag_init_component.hpp>
#include <ase/player/components/tag/player_tag_chunk_changed_component.hpp>

// Components - Request/Result (ECS-based spawn/despawn)
#include <ase/player/components/request/player_req_spawn_component.hpp>
#include <ase/player/components/request/player_req_desp_component.hpp>
#include <ase/player/components/request/player_req_spawn_res_component.hpp>
#include <ase/player/components/request/player_req_desp_res_component.hpp>

// Systems (focused, single responsibility)
#include <ase/player/systems/lifecycle/player_life_spawn_system.hpp>
#include <ase/player/systems/control/player_ctrl_input_system.hpp>
#include <ase/player/systems/control/player_ctrl_move_system.hpp>
#include <ase/player/systems/simulation/player_sim_phys_system.hpp>
#include <ase/player/systems/state/player_state_status_system.hpp>
#include <ase/player/systems/spatial/player_spatial_chunk_system.hpp>
#include <ase/player/systems/network/player_net_bct_req_system.hpp>
#include <ase/player/systems/network/player_net_bct_snd_system.hpp>
