#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_sta_phys_comp.hpp
 * @brief       PlayerStaPhysComponent - what physics currently holds true for this player
 * @description Bodenkontakt und Schwerkraftschalter: der gemessene Zustand, aus dem die
 *              Bewegungssysteme ableiten, ob gesprungen, gefallen oder gelaufen wird.
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
 * Schema-Nachzug von player_st_phys_component.hpp. Zwei Dinge stimmten nicht: das Legacy-Suffix
 * `_component` (Schema verlangt `_comp`) und das Kuerzel `st` mit nur zwei Zeichen - die
 * Taxonomie verlangt 3 bis 4, `state` heisst `sta` (WRFL_ASE_NAMING_SCHEMA). Weil der
 * Strukturname aus dem Dateinamen folgt, wandert er mit: PlayerStPhysComponent wird
 * PlayerStaPhysComponent. Die FELDER bleiben Zeichen fuer Zeichen dieselben.
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
 * [ ] Filename: player_sta_phys_comp.hpp (module FULL, tax 3-4 chars)
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

namespace ase::player {

/**
 * @brief PlayerStaPhysComponent - der physikalische Befund dieses Taktes
 *
 * Zwei Wahrheitswerte, und beide sind MESSUNGEN, keine Wuensche: ob die Figur den Boden beruehrt,
 * stellt die Simulation fest, und ob die Schwerkraft auf sie wirkt, ist eine Eigenschaft ihrer
 * Lage - nicht der Eingabe. Deshalb stehen sie hier und nicht bei den Eingabewerten.
 */
struct PlayerStaPhysComponent {
    bool on_ground = false;         // Bodenkontakt, von der Simulation festgestellt
    bool gravity_enabled = false;   // wirkt die Schwerkraft gerade auf diese Figur
};

}  // namespace ase::player
