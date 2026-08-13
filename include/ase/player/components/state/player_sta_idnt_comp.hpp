#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_sta_idnt_comp.hpp
 * @brief       PlayerStaIdntComponent - wer diese Figur ist und seit wann
 * @description Die dauerhafte Kennung einer Spielerfigur samt der beiden Zeitmarken, die ihre
 *              Sitzung umspannen: wann sie entstanden ist und wann zuletzt eine Eingabe von ihr
 *              kam. Alles, was eine Figur ueber Neustarts, Regionswechsel und Wiederverbindungen
 *              hinweg wiedererkennbar macht, steht hier.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state
 * @parity      server_only
 * @created     2025-12-25
 * @modified    2026-08-10
 * @version     00.00.00.00000 [seed]
 *
 * UMZUG MIT SYMBOL: `st` UND `id` SIND BEIDE ZU KURZ.
 *
 * Schema-Nachzug von player_st_id_component.hpp. Der Validator nannte drei Namensmaengel
 * (gemessen 2026-08-10, core/ase-validator/compliance/player/component/
 * player_st_id_component.hpp.cmplerr, Verstoss 5 und 6): das Legacy-Suffix `_component` statt
 * `_comp`, und ZWEI zu kurze Kuerzel - `st` und `id`. Das Schema verlangt 3 bis 4 Zeichen je
 * Taxonomieglied (WRFL_ASE_NAMING_SCHEMA). Die Quellen:
 *   `state`    → `sta`  (taxonomy/_index.json, {"abbrev": "sta", "name": "state"})
 *   `identity` → `idnt` (taxonomy/04_ent_entity.json:932; gleichlautend
 *                        25_sig_signature.json:218 und 06_str_structure.json:126 fuer
 *                        `identifier`, also kein Zufallstreffer einer Einzeldatei)
 * Weil der Strukturname aus dem Dateinamen folgt, wandert er mit: PlayerStIdComponent wird
 * PlayerStaIdntComponent. Die FELDER bleiben Zeichen fuer Zeichen dieselben.
 *
 * WARUM DIESE MIGRATION AUF DEM KRITISCHEN PFAD LIEGT: ase-codegen ruft mod_cmpl_check und
 * verwirft die GESAMTE Modulausgabe, solange auch nur ein Verstoss steht
 * (codegen_generate_json.py:check_ecs_compliance, Gate `violations == 0`; gemessen 2026-08-10
 * fuehrte der Lauf ase-player als `deleted`). Ohne diese Umzuege entsteht keine Client-Paritaet
 * fuer die SHARED-Komponenten dieses Moduls.
 *
 * WARUM DIE KENNUNG EIN FESTES ZEICHENFELD IST UND KEIN ZEIGER: 64 Byte liegen weit unter der
 * Grenze, ab der das Zeigermuster verlangt waere, und die Kennung reist mit der Figur ueber die
 * Regionsgrenze. Ein Zeiger waere dort wertlos - er zeigte auf den Speicher des ABSENDERS. Die
 * Wanderung traegt deshalb nicht die Zeichen selbst, sondern deren FNV-1a32-Streuwert als u32
 * (player_mig_ser_sys.cpp), und die Gegenseite fuehrt die Figur unter derselben Zahl weiter.
 *
 * ECS COMPONENT COMPLIANCE
 *
 * [ ] DATA fields ONLY - No methods
 * [ ] NO .cpp file - Header-only
 * [ ] ONLY zero-initialization (= 0, = 0.0f, = false, = {})
 * [ ] No magic numbers in defaults (use types.hpp constants)
 * [ ] Entity references initialized to = 0 (systems set values)
 * [ ] Single responsibility (one data category)
 * [ ] No God-Component (unrelated fields)
 * [ ] Large data uses pointer pattern (uint64_t ptr = 0)
 * [ ] Large data in registry.ctx()? (component has only lookup ID!)
 * [ ] Tag structs end with Tag suffix - N/A (not a tag)
 * [ ] Filename: player_sta_idnt_comp.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name derived from filename (snake_case to PascalCase)
 * [ ] 1 File = 1 Component
 * [ ] File in correct category subfolder
 * [ ] Per-entity runtime values use state/ components (NOT types.hpp!)
 * [ ] SHARED components listed in codegen.json components.shared
 * [ ] Pointer components in codegen.json components.server_only
 * [ ] Strings < 64 bytes use char[N] fixed arrays
 * [ ] Strings 64-256 bytes use appropriately sized char[N]
 * [ ] Strings > 256 bytes use Pointer Pattern (uint64_t ptr, uint16_t len)
 * [ ] Strings > 256 bytes use registry.ctx() mit Lookup-ID?
 * [ ] NO Entity-per-Character (strings are single attributes, not N-Items!)
 * [ ] Lookup-only strings use uint32_t hash (entt::hashed_string)
 * [ ] NO std::shared_ptr in components (use Flyweight Pattern via ctx!)
 * [ ] NO void* in components (use Flyweight Pattern via ctx!)
 * [ ] NO uint64_t as pointer concept (use uint32_t ID + ResourceManager via ctx!)
 * [ ] External library objects (shared_ptr, handles) in ResourceManager via ctx()
 * [ ] Component stores ONLY primitive ID (uint32_t) referencing external resource
 */

#include <cstdint>

namespace ase::player {

/**
 * @brief PlayerStaIdntComponent - Kennung und Sitzungszeitmarken einer Spielerfigur
 *
 * player_id     = Fixed-size player identifier (UUID format, max 36 chars + null)
 * spawned_at_ms = Timestamp in milliseconds when player spawned (steady_clock epoch)
 * last_input_ms = Timestamp in milliseconds of last input (steady_clock epoch)
 *
 * Die beiden Zeitmarken stehen NEBEN der Kennung und nicht in einer eigenen Komponente, weil sie
 * denselben Sachverhalt beschreiben: die Lebensspanne genau dieser Kennung. Wer wissen will, ob
 * eine Figur noch mitspielt, vergleicht die letzte Eingabe gegen die Uhr - dafuer beides in
 * einem Zugriff zu haben, ist kein Sammelbehaelter, sondern derselbe Gegenstand.
 */
struct PlayerStaIdntComponent {
    char player_id[64] = {};      // dauerhafte Kennung der Figur (UUID-Text, 36 Zeichen + Null)
    uint64_t spawned_at_ms = 0;   // Entstehungszeitpunkt der Figur (ms, steady_clock-Epoche)
    uint64_t last_input_ms = 0;   // letzte Eingabe dieser Figur (ms, steady_clock-Epoche)
};

}  // namespace ase::player
