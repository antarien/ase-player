#pragma once

/**
 * ASE ECS COMPONENT (REQUEST)
 *
 * @file        player_req_spwn_comp.hpp
 * @brief       PlayerReqSpwnComponent - der Wunsch, eine Figur entstehen zu lassen
 * @description Traegt alles, was eine Entstehung braucht: unter welcher Kennung die Figur
 *              gefuehrt wird und an welcher Stelle der Welt sie erscheinen soll.
 *              PlayerLifeSpwnSystem liest den Wunsch und legt den Bescheid daneben.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    communication/request
 * @parity      server_only
 * @created     2025-12-25
 * @modified    2026-08-10
 * @version     00.00.00.00000 [seed]
 * @author      Jan Ohlmann (ADG/ASE/AOW)
 *
 * UMZUG MIT SYMBOL: `spawn` IST ZU LANG.
 *
 * Schema-Nachzug von player_req_spawn_component.hpp. Der Validator nannte dieselben Maengel wie
 * beim Bescheid (gemessen 2026-08-10, core/ase-validator/compliance/player/component/
 * player_req_spawn_component.hpp.cmplerr): Legacy-Suffix `_component` statt `_comp`, das
 * ausgeschriebene `spawn` - die Taxonomie erlaubt hoechstens 4 Zeichen und fuehrt `spwn` in DREI
 * Kategorien (03_act_action.json:14, 11_bio_biology.json:507, 29_ecs_ecs.json:32) - und
 * `@category request`, das als Pfad nicht existiert; gueltig ist `communication/request`.
 * PlayerReqSpawnComponent wird PlayerReqSpwnComponent. Die FELDER bleiben dieselben.
 *
 * WARUM DIESE MIGRATION AUF DEM KRITISCHEN PFAD LIEGT: ase-codegen ruft mod_cmpl_check und
 * verwirft die GESAMTE Modulausgabe, solange auch nur ein Verstoss steht
 * (codegen_generate_json.py:check_ecs_compliance, Gate `violations == 0`; gemessen 2026-08-10
 * fuehrte der Lauf ase-player als `deleted`).
 *
 * NUR ZWEI ACHSEN, UND DAS MIT ABSICHT: der Wunsch nennt x und z, nicht die Hoehe. Die Wabe,
 * in der die Figur landet, spannen X und Z auf (player_life_spwn_sys.cpp:330-331); die Hoehe
 * ergibt sich aus dem Gelaende und ist deshalb ein BEFUND der Simulation, kein Wunsch des
 * Aufrufers. Wer eine Hoehe vorgeben koennte, koennte eine Figur in den Fels setzen.
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
 * [ ] Filename: player_req_spwn_comp.hpp (module FULL, tax 3-4 chars)
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
 * @brief PlayerReqSpwnComponent - Kennung und Stelle einer gewuenschten Entstehung
 *
 * Die Kennung steht als Zeichenfeld und nicht als Streuwert, weil sie beim Entstehen ERSTMALS
 * vergeben wird: ein Streuwert liesse sich hinterher nicht mehr in den Klartext zurueckrechnen,
 * unter dem die Figur in der Ablage gefuehrt wird. Erst spaeter, wenn sie ueber eine
 * Regionsgrenze reist, tritt der Streuwert an ihre Stelle.
 */
struct PlayerReqSpwnComponent {
    char player_id[64] = {};  // Kennung, unter der die Figur gefuehrt wird (UUID-Text)
    float x = 0.0f;           // gewuenschte Stelle, Weltachse X in Metern
    float z = 0.0f;           // gewuenschte Stelle, Weltachse Z in Metern
};

}  // namespace ase::player
