#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_drty_tag.hpp
 * @brief       PlayerDrtyTag - this player changed since the last outbound pass
 * @description Der Sende- und Anzeigepfad arbeitet nur an markierten Entitaeten; ohne Marke gilt
 *              der zuletzt uebertragene Stand als gueltig und niemand wiederholt ihn.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    tag
 * @parity      server_only
 * @created     2025-12-25
 * @modified    2026-08-10
 * @version     00.00.00.00000 [seed]
 *
 * UMZUG MIT SYMBOL, KUERZEL AUS DER TAXONOMIE.
 *
 * Schema-Nachzug von player_tag_dirty_component.hpp. Der Typ-Marker stand als `_tag_` in der
 * MITTE statt als Suffix, und `dirty` ist mit fuenf Zeichen zu lang (max 4,
 * WRFL_ASE_NAMING_SCHEMA). Die Taxonomie-Tabelle fuehrt "dirty": "drty" - nachgeschlagen, nicht
 * geraten. Weil der Strukturname aus dem Dateinamen folgt, wandert er mit: PlayerDirtyTag wird
 * PlayerDrtyTag, ein leeres Marker-Struct, inhaltlich unveraendert.
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
 * [ ] Filename: player_drty_tag.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: PlayerDrtyTag (PascalCase)
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
 * @brief PlayerDrtyTag - seit dem letzten Ausgang veraendert
 *
 * Der Marker ersetzt den Vergleich: ohne ihn muesste jeder ausgehende Pass den alten Stand
 * mitfuehren und Feld fuer Feld gegenpruefen. Wer schreibt, setzt ihn; wer gesendet hat, nimmt
 * ihn ab. Damit ist "hat sich etwas geaendert" eine View-Frage und keine Rechnung.
 */
struct PlayerDrtyTag {};

}  // namespace ase::player
