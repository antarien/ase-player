#pragma once
/**
 * PlayerMgrTag - Marker for the player manager entity
 *
 * Used to identify the singleton entity that holds PlayerStMovComponent.
 * Created by PlayerLifeSpawnSystem::on_start.
 */

namespace ase::player {

/**
 * Tag for player manager entity (holds configuration)
 */
struct PlayerMgrTag {};

}  // namespace ase::player
