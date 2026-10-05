#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_sess_strt_tag.hpp
 * @brief       PlayerSessStrtTag - this player's session start has been raised on the Hub
 * @description PlayerHubSessStrtSystem sets the contract topic PLAYER_SESSION_START to 1 for a
 *              player it sees for the first time and marks the player with this tag (Phase 13,
 *              register D0703; the topic is CoordinationDetector's subscription). The tag is the
 *              half-way state of a PULSE: a session start is an event, not a state, so the flag must
 *              go out in one dissemination pass and come back down in the next. The tag says "the
 *              rising edge is staged"; PlayerSessDoneTag says "the falling edge is staged too".
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    tag
 * @parity      server_only
 * @created     2026-10-03
 * @modified    2026-10-03
 * @version     1.0.0
 *
 * sess = session, strt = start (taxonomy catalogue)
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
 * @brief PlayerSessStrtTag - the session start of this player is up on the Hub
 *
 * Present from the pass that raised PLAYER_SESSION_START to 1. Its absence on a player is the
 * question "has this player's session start been announced?", answered by a View filter.
 */
struct PlayerSessStrtTag {};

}  // namespace ase::player
