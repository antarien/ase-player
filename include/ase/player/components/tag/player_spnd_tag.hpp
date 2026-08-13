#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_spnd_tag.hpp
 * @brief       PlayerSpndTag - this entity is a player that has actually entered the world
 * @description Trennt die eingesetzte Spielfigur von jeder anderen Entitaet: Kamera, Migration,
 *              Persistenz und Beobachtung filtern ueber diesen Marker statt ueber ein Feld.
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
 * Schema-Nachzug von player_tag_spawned_component.hpp. Der Typ-Marker stand als `_tag_` in der
 * MITTE statt als Suffix, und `spawned` ist mit sieben Zeichen zu lang (max 4,
 * WRFL_ASE_NAMING_SCHEMA). Die Taxonomie-Tabelle fuehrt "spawned": "spnd" - nachgeschlagen, nicht
 * geraten; das benachbarte "spwn" gehoert zu `spawn`, dem Vorgang, nicht zum Zustand danach. Weil
 * der Strukturname aus dem Dateinamen folgt, wandert er mit: PlayerSpawnedTag wird PlayerSpndTag,
 * ein leeres Marker-Struct, inhaltlich unveraendert.
 *
 * DIE WEITESTE EINHEIT DIESER MIGRATION: der Marker wird ausserhalb des eigenen Moduls gelesen -
 * ase-world (Migration, Bindung) und ase-camera (Zielwahl) haengen daran. Cross-Modul-Lesen eines
 * Tags ist zulaessig (der Header ist L3-Vertrag), aber es macht diesen Umzug zu dem einen, bei dem
 * ein vergessener Aufrufer erst beim Bauen auffiele.
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
 * [ ] Filename: player_spnd_tag.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: PlayerSpndTag (PascalCase)
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
 * @brief PlayerSpndTag - die Figur steht in der Welt
 *
 * "Spieler sein" ist keine Eigenschaft, die man in ein Feld schreibt, sondern eine Zugehoerigkeit
 * zu einer Menge - und Mengen bildet dieses ECS mit Tags. Wer die eingesetzten Figuren braucht,
 * nimmt den Marker in seine View auf und sieht dadurch weder halbfertige Anfragen noch
 * Verwalter-Entitaeten.
 */
struct PlayerSpndTag {};

}  // namespace ase::player
