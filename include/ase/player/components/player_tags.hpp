#pragma once

/**
 * Player Tag Components - Empty structs for ECS filtering
 *
 * Tags have no data - only used for entity selection in views.
 * EnTT optimizes these with Empty Type Optimization (ETO).
 */

namespace ase::player {

/** Player entity that needs network broadcast */
struct PlayerDirtyTag {};

/** Newly spawned player (broadcast spawn event) */
struct PlayerJustSpawnedTag {};

/** Local player (controlled by this server's session) */
struct LocalPlayerTag {};

/** Player needs initialization */
struct PlayerNeedsInitTag {};

/** Player chunk changed (for Neo4j LOCATED_IN update) */
struct PlayerChunkChangedTag {};

}  // namespace ase::player
