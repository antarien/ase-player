#pragma once
/**
 * PlayerMgrTag - Marker for the player manager/config entity
 *
 * Used to identify the singleton entity that holds PlayerStateCfgComponent.
 * Created by PlayerLifeSpawnSystem::on_start.
 */

namespace ase::player {

/**
 * Tag for player manager entity (holds configuration)
 */
struct PlayerMgrTag {};

}  // namespace ase::player
