#pragma once

/**
 * PlayerBufPstComponent - Player Persistence Buffer (POD)
 *
 * Contains serialized player data for MongoDB persistence.
 * Attached to player entities that need persistence.
 *
 * Pattern (Star Citizen Replication Layer):
 * 1. PlayerPstSerSystem serializes player state into this buffer
 * 2. Sets ReplicationPstMetComponent + ReplicationDtyTag
 * 3. ReplicationPstSystem handles MongoDB save via Replication Layer
 *
 * Naming: Player + Buf(Buffer) + Pst(Persist) + Component
 */

#include <cstdint>

namespace ase::player {

struct PlayerBufPstComponent {
    // JSON document for MongoDB (pointer pattern)
    uint64_t jsn_ptr = 0;      // Pointer to JSON string
    uint32_t jsn_len = 0;      // JSON string length

    // Player identification (pointer pattern)
    uint64_t plr_id_ptr = 0;   // Pointer to player ID string
    uint32_t plr_id_len = 0;   // Player ID length

    // Serialization state
    uint8_t st = 0;            // 0=pending, 1=serializing, 2=done, 3=error
};

}  // namespace ase::player
