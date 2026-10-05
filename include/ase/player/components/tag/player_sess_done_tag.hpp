#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_sess_done_tag.hpp
 * @brief       PlayerSessDoneTag - this player's session-start pulse has been lowered again
 * @description The closing state of the PLAYER_SESSION_START pulse (Phase 13, register D0703):
 *              PlayerHubSessStrtSystem drops the flag back to 0 one dissemination pass after it
 *              raised it and marks the player with this tag, so the player is never announced
 *              twice. Without the second edge the Replica would keep its trigger row alive and
 *              re-fire CoordinationDetector every heartbeat for as long as the player plays —
 *              a session start would be billed like a sustained cheat.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    tag
 * @parity      server_only
 * @created     2026-10-03
 * @modified    2026-10-03
 * @version     1.0.0
 *
 * sess = session (taxonomy catalogue)
 *
 * ECS TAG COMPLIANCE
 *
 * [ ] DATA fields ONLY - No methods (empty struct for tags)
 * [ ] NO .cpp file - Header-only
 * [ ] ONLY zero-initialization - N/A (no fields)
 * [ ] No magic numbers in defaults - N/A (no fields)
 * [ ] Entity references - N/A (no fields)
 * [ ] Single responsibility - N/A (marker only)
 * [ ] No God-Component - N/A (no fields)
 * [ ] Large data uses pointer pattern - N/A (no data)
 * [ ] Large data in registry.ctx() - N/A (Tags have no data)
 * [ ] Tag structs end with Tag suffix
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: Remove tag_ from middle, add Tag suffix
 * [ ] 1 File = 1 Component
 * [ ] File in tag/ subfolder (with optional deeper hierarchy)
 * [ ] Per-entity runtime values use state/ components (NOT types.hpp!)
 * [ ] SHARED components listed in codegen.json components.shared
 * [ ] Pointer components in codegen.json components.server_only
 * [ ] Tag replaces `bool is_*` or `bool has_*` field in Component
 * [ ] Tag replaces `uint8_t *_type` field with if-chain dispatch
 * [ ] Systems use View filter instead of if-else inside loop
 * [ ] INCLUDE: registry.view<Component, ThisTag>()
 * [ ] EXCLUDE: registry.view<Component>(entt::exclude<ThisTag>)
 * [ ] NO if (entity.has<Tag>) inside loop - use filtered View!
 * [ ] NO switch/case on type - use separate View per Tag!
 * [ ] Each state = separate Tag + separate View in System
 * [ ] N-item support via Entity-per-Item + Tags
 */

namespace ase::player {

/**
 * @brief PlayerSessDoneTag - the session-start pulse of this player is complete
 *
 * Present from the pass that lowered PLAYER_SESSION_START back to 0. A player carrying it is
 * excluded from both views of PlayerHubSessStrtSystem for the rest of its life.
 */
struct PlayerSessDoneTag {};

}  // namespace ase::player
