#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_sta_chk_comp.hpp
 * @brief       PlayerStaChkComponent - the chunk this player currently stands in
 * @description Die Chunk-Adresse der Figur, abgeleitet aus ihrer Position: sie entscheidet, wer
 *              den Spieler simuliert, welche Region ihn haelt und welche Wabe er begeht.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state
 * @parity      server_only
 * @created     2025-12-25
 * @modified    2026-08-10
 * @version     00.00.00.00000 [seed]
 *
 * UMZUG MIT SYMBOL: `st` IST ZU KURZ.
 *
 * Schema-Nachzug von player_st_chk_component.hpp. Zwei Dinge stimmten nicht: das Legacy-Suffix
 * `_component` (Schema verlangt `_comp`) und das Kuerzel `st` mit nur zwei Zeichen - die
 * Taxonomie verlangt 3 bis 4, `state` heisst `sta` (WRFL_ASE_NAMING_SCHEMA). Weil der
 * Strukturname aus dem Dateinamen folgt, wandert er mit: PlayerStChkComponent wird
 * PlayerStaChkComponent. Die FELDER bleiben Zeichen fuer Zeichen dieselben.
 *
 * WEIT GELESEN: ase-world haengt an dieser Adresse - Regionslast, Grenzwacht und Migration
 * bestimmen darueber, wer die Figur simuliert. Ein vergessener Aufrufer fiele erst beim Bauen auf,
 * deshalb wurden alle Fundstellen VOR dem Umzug gemessen.
 *
 * WARUM DIESE MIGRATION AUF DEM KRITISCHEN PFAD LIEGT: ase-codegen ruft mod_cmpl_check und
 * verwirft die GESAMTE Modulausgabe, solange auch nur ein Verstoss steht
 * (codegen_generate_json.py:check_ecs_compliance, Gate `violations == 0`; gemessen 2026-08-10
 * fuehrte der Lauf ase-player als `deleted`). Ohne diese Umzuege entsteht keine Client-Paritaet
 * fuer die SHARED-Komponenten dieses Moduls.
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
 * [ ] Filename: player_sta_chk_comp.hpp (module FULL, tax 3-4 chars)
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
 * @brief PlayerStaChkComponent - die Chunk-Adresse der Figur
 *
 * Vorzeichenbehaftet, und das ist keine Kleinigkeit: die Weltachse laeuft in beide Richtungen um
 * den Nullpunkt, ein Chunk westlich oder suedlich davon traegt eine NEGATIVE Adresse. Wer das
 * vorzeichenlos rechnet, schickt die Figur an den anderen Rand der Zahlenwelt.
 */
struct PlayerStaChkComponent {
    int32_t chunk_x = 0;  // Chunk-Adresse auf der X-Achse (negativ westlich des Nullpunkts)
    int32_t chunk_y = 0;  // Chunk-Adresse auf der zweiten Bodenachse
};

}  // namespace ase::player
