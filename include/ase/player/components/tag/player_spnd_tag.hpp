#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_spnd_tag.hpp
 * @brief       PlayerSpndTag - this entity is a player that has actually entered the world
 * @description Trennt die eingesetzte Spielfigur von jeder anderen Entitaet: Persistenz,
 *              Beobachtung und die Hub-Zeile HUB_PLR_SPND filtern ueber diesen Marker statt ueber
 *              ein Feld.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    tag
 * @parity      server_only
 * @usecase     usecase:5130 - play a character
 * @actor       SS/PC
 * @created     2025-12-25
 * @modified    2026-10-13
 * @version     00.00.00.00000 [seed]
 *
 * KUERZEL AUS DER TAXONOMIE.
 *
 * Die Taxonomie-Tabelle fuehrt "spawned": "spnd" - nachgeschlagen, nicht geraten; das benachbarte
 * "spwn" gehoert zu `spawn`, dem Vorgang, nicht zum Zustand danach (WRFL_ASE_NAMING_SCHEMA).
 *
 * DIESER MARKER BLEIBT IM MODUL. Ihn setzen die beiden Stellen, an denen eine Figur die Welt
 * betritt - PlayerLifeSpwnSystem und PlayerMigDesSystem -, und PlayerHubSpndSystem traegt jede
 * markierte Figur als Hub-Zeile HUB_PLR_SPND auf den Stern. Fremde Module lesen die ZEILE ueber
 * hub::row_each und hub::row_has, nie diesen Typ: ase-bdi (keine zweite Seele fuer eine Sitzung),
 * ase-camera (Zielwahl), ase-character, ase-geoid und ase-world (Bindung, Migration, Last).
 *
 * WARUM DIE BENENNUNG AUF DEM KRITISCHEN PFAD LIEGT: ase-codegen ruft mod_cmpl_check und
 * verwirft die GESAMTE Modulausgabe, solange auch nur ein Verstoss steht
 * (codegen_generate_json.py:check_ecs_compliance, Gate `violations == 0`). Ohne einen
 * schemagerechten Namen entsteht keine Client-Paritaet fuer die SHARED-Komponenten dieses Moduls.
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
 * zu einer Menge - und Mengen bildet dieses ECS mit Tags. Wer im Modul die eingesetzten Figuren
 * braucht, nimmt den Marker in seine View auf und sieht dadurch weder halbfertige Anfragen noch
 * Verwalter-Entitaeten.
 */
struct PlayerSpndTag {};

}  // namespace ase::player
