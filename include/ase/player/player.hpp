#pragma once

/**
 * ASE Player Module
 *
 * Provides player entity management using focused ECS components.
 *
 * Components (focused, single responsibility):
 *   - PlayerIdentityComponent: Player ID and session timestamps
 *   - PlayerPositionComponent: World position and rotation
 *   - PlayerVelocityComponent: Current velocity vector
 *   - PlayerPhysicsComponent: Ground contact and physics flags
 *   - PlayerStateComponent: State machine (Idle, Walking, Running...)
 *   - PlayerChunkComponent: Current chunk position
 *   - PlayerConfigComponent: Singleton with height query callback
 *   - Tags: PlayerDirtyTag, PlayerJustSpawnedTag, LocalPlayerTag, etc.
 *
 * Request Components (for spawn/despawn via ECS):
 *   - PlayerSpawnRequestComponent: Request to spawn a player
 *   - PlayerDespawnRequestComponent: Request to despawn a player
 *   - PlayerSpawnResultComponent: Result of spawn request
 *   - PlayerDespawnResultComponent: Result of despawn request
 *
 * Systems:
 *   - PlayerLifecycleSystem: Process spawn/despawn requests
 *   - PlayerInputSystem: Process WebRTC input
 *   - PlayerMovementSystem: Calculate velocity from input
 *   - PlayerPhysicsSystem: Apply gravity, terrain collision
 *   - PlayerStateSystem: Update state machine
 *   - PlayerChunkSystem: Detect chunk changes
 *   - PlayerBroadcastSystem: Broadcast dirty entities via ECS message pattern
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

// Components (focused, single responsibility)
#include <ase/player/components/player_identity_component.hpp>
#include <ase/player/components/player_position_component.hpp>
#include <ase/player/components/player_velocity_component.hpp>
#include <ase/player/components/player_physics_component.hpp>
#include <ase/player/components/player_state_component.hpp>
#include <ase/player/components/player_chunk_component.hpp>
#include <ase/player/components/player_config_component.hpp>
#include <ase/player/components/player_tags.hpp>

// Request/Result Components (ECS-based spawn/despawn)
#include <ase/player/components/player_spawn_request_component.hpp>
#include <ase/player/components/player_despawn_request_component.hpp>
#include <ase/player/components/player_spawn_result_component.hpp>
#include <ase/player/components/player_despawn_result_component.hpp>

// Systems (focused, single responsibility)
#include <ase/player/systems/player_lifecycle_system.hpp>
#include <ase/player/systems/player_input_system.hpp>
#include <ase/player/systems/player_movement_system.hpp>
#include <ase/player/systems/player_physics_system.hpp>
#include <ase/player/systems/player_state_system.hpp>
#include <ase/player/systems/player_chunk_system.hpp>
#include <ase/player/systems/player_broadcast_system.hpp>
