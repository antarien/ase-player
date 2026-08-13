#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_sta_vel_comp.hpp
 * @brief       PlayerStaVelComponent - wie schnell und wohin sich die Figur gerade bewegt
 * @description Der Geschwindigkeitsvektor einer Spielerfigur in Metern je Sekunde, Weltachsen.
 *              Bewegungsabsicht und Schwerkraft schreiben ihn, die Zustandsauswertung liest
 *              daraus die Haltung, und die Wanderungsserialisierung nimmt ihn ueber die
 *              Regionsgrenze mit.
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
 * Schema-Nachzug von player_st_vel_component.hpp. Der Validator nannte GENAU zwei Namensmaengel
 * (gemessen 2026-08-10, core/ase-validator/compliance/player/component/
 * player_st_vel_component.hpp.cmplerr, Verstoss 7 und 8): das Legacy-Suffix `_component` statt
 * `_comp`, und die zweizeichige Abkuerzung `st` - das Schema verlangt 3 bis 4, `state` heisst
 * `sta` (core/ase-validator/ecs_validator/data/taxonomy/_index.json, Eintrag
 * {"abbrev": "sta", "name": "state"}). `vel` fuer velocity wurde NICHT beanstandet und bleibt.
 * Weil der Strukturname aus dem Dateinamen folgt, wandert er mit: PlayerStVelComponent wird
 * PlayerStaVelComponent. Die FELDER bleiben Zeichen fuer Zeichen dieselben.
 *
 * WARUM DIESE MIGRATION AUF DEM KRITISCHEN PFAD LIEGT: ase-codegen ruft mod_cmpl_check und
 * verwirft die GESAMTE Modulausgabe, solange auch nur ein Verstoss steht
 * (codegen_generate_json.py:check_ecs_compliance, Gate `violations == 0`; gemessen 2026-08-10
 * fuehrte der Lauf ase-player als `deleted`). Ohne diese Umzuege entsteht keine Client-Paritaet
 * fuer die SHARED-Komponenten dieses Moduls.
 *
 * WEITREICHEND: die drei Werte reisen als f32-Tripel im eingefrorenen PlayerSnap
 * (ase/types/region_wire.hpp) ueber die Regionsgrenze - eine rennende Figur rennt drueben
 * weiter. Der Umzug fasst nur den NAMEN an, nie die Bytefolge des Rahmens.
 *
 * ACHSENFALLE, GEMESSEN 2026-08-10: in DIESEM Modul ist Y die Senkrechte. Die Physik nullt vy
 * beim Bodenkontakt (player_sim_phys_sys.cpp:231) und rechnet das Tempo am Boden aus
 * sqrt(vx^2 + vz^2) (:240); das Wabenraster spannen X und Z auf
 * (player_life_spwn_sys.cpp:330-331). Auf der Geoid-/GIS-Seite ist dagegen Z die Hoehenachse.
 * Wer Werte zwischen beiden Welten traegt, TAUSCHT also - er kopiert nicht.
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
 * [ ] Filename: player_sta_vel_comp.hpp (module FULL, tax 3-4 chars)
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
 * @brief PlayerStaVelComponent - die Geschwindigkeit dieses Taktes, in Weltachsen
 *
 * Getrennt von der Lage (PlayerStaPosComponent) und getrennt von der Absicht
 * (PlayerStaInpMovComponent), weil drei verschiedene Systeme sie schreiben: die Bewegung setzt
 * die waagerechten Anteile, die Physik den senkrechten, und der Empfang einer Wanderung setzt
 * alle drei auf einmal. Ein gemeinsamer Behaelter fuer Lage und Geschwindigkeit wuerde diese
 * drei Schreiber gegeneinander laufen lassen.
 */
struct PlayerStaVelComponent {
    float vx = 0.0f;  // waagerecht, Weltachse X (m/s)
    float vy = 0.0f;  // SENKRECHT - hier greift die Schwerkraft an (m/s)
    float vz = 0.0f;  // waagerecht, Weltachse Z (m/s)
};

}  // namespace ase::player
