#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_locl_tag.hpp
 * @brief       PlayerLoclTag - this player belongs to the session sitting in front of the screen
 * @description Unterscheidet den eigenen Spieler von allen anderen, ohne dass ein Feld die Rolle
 *              traegt: Kamera, Eingabe und Anzeige filtern ueber diesen Marker.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    tag
 * @parity      server_only
 * @created     2025-12-25
 * @modified    2026-08-10
 * @version     00.00.00.00000 [seed]
 *
 * UMZUG MIT SYMBOL, UND DAS KUERZEL KOMMT AUS DER TAXONOMIE, NICHT AUS DEM BAUCH.
 *
 * Schema-Nachzug von player_tag_local_component.hpp. Zwei Dinge stimmten nicht: der Typ-Marker
 * stand als `_tag_` in der MITTE statt als Suffix, und `local` ist mit fuenf Zeichen ein zu
 * langes Taxonomie-Kuerzel. Der erste Versuch `lcl` wurde abgewiesen - die Taxonomie-Tabelle
 * fuehrt `"local": "locl"` (core/ase-validator/ecs_validator/data/taxonomy/, fuenf Fundstellen).
 * Der Vorschlagstext des Validators nannte `lcl` und dann `clcl`; massgeblich ist die Tabelle,
 * nicht der Vorschlag. Weil der Strukturname aus dem Dateinamen folgt, wandert er mit:
 * PlayerLocalTag wird PlayerLoclTag - ein leeres Marker-Struct, inhaltlich unveraendert.
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
 * [ ] Filename: player_locl_tag.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: PlayerLoclTag (PascalCase)
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
 * @brief PlayerLoclTag - der eigene Spieler dieser Sitzung
 *
 * "Eigen" ist eine Rolle, kein Merkmal der Figur - deshalb ein Tag und kein Feld. Ein `bool
 * is_local` auf der Komponente wuerde dieselbe Figur in zwei Sitzungen widerspruechlich
 * beschreiben; der Marker gehoert der Sitzung, die ihn setzt.
 */
struct PlayerLoclTag {};

}  // namespace ase::player
