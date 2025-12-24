#pragma once

/**
 * PlayerPstDtyTag - Player needs persistence to database
 *
 * Set when player state changes significantly (position, status).
 * Processed by PlayerPstSerSystem which sets ReplicationDtyTag.
 */

namespace ase::player {

struct PlayerPstDtyTag {};

}  // namespace ase::player
