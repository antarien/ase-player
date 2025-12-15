#pragma once

/**
 * ASE Player Module
 *
 * Provides player entity management, movement, and replication.
 *
 * Components:
 *   - PlayerComponent: Core player state (position, velocity, yaw)
 *   - PlayerInput: Buffered client input
 *   - PlayerChunkPresence: Current chunk tracking
 *
 * Systems:
 *   - PlayerSystem: Movement, gravity, terrain collision
 *
 * Usage:
 *   auto* player_sys = world.get_system<PlayerSystem>();
 *   player_sys->spawn_player(registry, "player123", {16, 0, 16});
 *   player_sys->apply_input(registry, "player123", input);
 */

#include <ase/player/types.hpp>
#include <ase/player/components/player_component.hpp>
#include <ase/player/systems/player_system.hpp>
