#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_roam_run_tag.hpp
 * @brief       PlayerRoamRunTag - the errand is walked in the running gear, not the walking one
 * @description Gear tag of a backend-driven player. The movement system knows two gears and the
 *              requested speed decides which one an errand needs; that decision is made ONCE,
 *              when the errand is born, and stands as a tag rather than a field. The hub writer
 *              then publishes PLR_INP_SPRINT from two tag-filtered views and never compares a
 *              speed to a threshold in a loop.
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
 * [ ] Filename: player_roam_run_tag.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: PlayerRoamRunTag (PascalCase)
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
 * @brief PlayerRoamRunTag - the errand is walked in the running gear
 *
 * State: the requested walking speed is more than the walking gear carries, so the errand rides
 *        the running gear - still under the movement authority, never above it
 * Filter: PlayerHubRoamSystem publishes PLR_INP_SPRINT from the view that carries this tag and
 *         the view that excludes it
 * Added: PlayerLifeSpwnSystem when the errand is born with a speed above the walking gear
 * Removed: never - the gear of an errand is decided once, with the errand itself
 */
struct PlayerRoamRunTag {};

}  // namespace ase::player
