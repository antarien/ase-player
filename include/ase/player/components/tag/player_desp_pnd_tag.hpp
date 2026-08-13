#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_desp_pnd_tag.hpp
 * @brief       PlayerDespPndTag - this player is due to be despawned
 * @description Traegt die Absicht bis zum Aufraeumschritt: die Entitaet wird NIE mitten in der
 *              Iteration zerstoert, die Zerstoerung hat ihren eigenen Pass.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    tag
 * @parity      server_only
 * @created     2025-12-25
 * @modified    2026-08-10
 * @version     00.00.00.00000 [seed]
 *
 * DER TYP-MARKER GEHOERT ANS ENDE, NICHT IN DIE MITTE.
 *
 * Schema-Nachzug von player_tag_desp_pnd_component.hpp. Der alte Name trug `_tag_` als Wortteil
 * in der MITTE und dazu das Legacy-Suffix `_component`; das Schema verlangt den Typ-Marker als
 * SUFFIX (WRFL_ASE_NAMING_SCHEMA), also player_desp_pnd_tag.hpp. Der STRUKTURNAME war bereits
 * korrekt und bleibt unveraendert - PlayerDespPndTag.
 *
 * WARUM DIESE MIGRATION AUF DEM KRITISCHEN PFAD LIEGT: ase-codegen ruft mod_cmpl_check und
 * verwirft die GESAMTE Modulausgabe, solange auch nur ein Verstoss steht
 * (codegen_generate_json.py:check_ecs_compliance, Gate `violations == 0`; gemessen 2026-08-10
 * fuehrte der Lauf ase-player als `deleted`). Ohne diese Umzuege entsteht keine Client-Paritaet
 * fuer die SHARED-Komponenten dieses Moduls.
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
 * [ ] Filename: player_desp_pnd_tag.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: PlayerDespPndTag (PascalCase)
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
 * @brief PlayerDespPndTag - Abgang angemeldet, noch nicht vollzogen
 *
 * Zwischen Entscheidung und Zerstoerung liegt genau ein Pass, und das ist kein Umweg: eine
 * Entitaet waehrend der Iteration ihrer eigenen View zu zerstoeren macht Referenzen ungueltig.
 * Der Tag haelt die Absicht fest, bis der Aufraeumschritt sie gefahrlos ausfuehren kann.
 */
struct PlayerDespPndTag {};

}  // namespace ase::player
