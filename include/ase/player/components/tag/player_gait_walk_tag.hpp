#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_gait_walk_tag.hpp
 * @brief       PlayerGaitWalkTag - the figure walks: its horizontal speed lies from the stand bound up to the walk bound
 * @description Gait tag of a player figure. Standing, walking and running are three tags rather
 *              than three values of one field, so a clip mixer or a counter reads the gait from a
 *              tag-filtered view and never compares a number. Exactly one of the three gait tags
 *              stands on a figure once PlayerStaGaitSystem has run.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    tag
 * @parity      shared
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-13
 * @modified    2026-11-13
 * @version     00.00.00.00000 [seed]
 *
 * KUERZEL AUS DER TAXONOMIE: "gait": "gait", "walk": "walk" (data/taxonomy), nachgeschlagen.
 *
 * DIESER MARKER BLEIBT IM MODUL. Fremde Module und der Adapter lesen die Gangart als Hub-Wert,
 * den die Hub-Sync-Systeme von ase-pl-avatar aus diesen drei Markern schreiben, nie diesen Typ.
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
 * [ ] Filename: player_gait_walk_tag.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: PlayerGaitWalkTag (PascalCase)
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
 * @brief PlayerGaitWalkTag - die Figur geht
 *
 * State: das waagerechte Tempo liegt ab PLAYER_GAIT_STND_SPEED_MAX bis einschliesslich PLAYER_GAIT_WALK_SPEED_MAX
 * Filter: der Geh-Clip, die Gangzaehler von PlayerLogObsvSystem
 * Added: PlayerStaGaitSystem, aus der View, die diesen Marker ausschliesst, sobald das Tempo im Gehen liegt
 * Removed: PlayerStaGaitSystem, sobald das Tempo ruht oder ins Laufen steigt
 */
struct PlayerGaitWalkTag {};

}  // namespace ase::player
