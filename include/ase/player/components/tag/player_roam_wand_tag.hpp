#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_roam_wand_tag.hpp
 * @brief       PlayerRoamWandTag - the walker is on a leg, holding one heading
 * @description Phase tag of a backend-driven player's journey. While it is present the walker
 *              holds the heading of PlayerStaRoamComponent and feeds a forward input; when the
 *              leg clock runs out the walk hands over to PlayerRoamRestTag. The two phases are
 *              tags with one system each - never a phase code in the row and never a branch in a
 *              loop.
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    tag
 * @created     2026-08-09
 * @modified    2026-08-09
 * @version     1.0.0
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
 * [ ] Filename: player_roam_wand_tag.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: PlayerRoamWandTag (PascalCase)
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
 * @brief PlayerRoamWandTag - the walker is on a leg, holding one heading
 *
 * State: the player is walking the current leg of its journey on the heading its errand row
 *        carries; the forward input is live
 * Filter: PlayerSimWandSystem iterates exactly this state
 * Added: PlayerSimRestSystem when a pause ends and the next heading has been decided
 * Removed: PlayerSimWandSystem when the leg clock reaches zero - the walker rests
 */
struct PlayerRoamWandTag {};

}  // namespace ase::player
