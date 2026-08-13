#pragma once

/**
 * ASE ECS COMPONENT (REQUEST)
 *
 * @file        player_req_spwn_res_comp.hpp
 * @brief       PlayerReqSpwnResComponent - der Bescheid auf einen Entstehungswunsch
 * @description Die Antwort, die PlayerLifeSpwnSystem auf die Wunsch-Entitaet legt, nachdem es
 *              den Entstehungswunsch bearbeitet hat: ob es geklappt hat, und welche Figur dabei
 *              entstanden ist. Wer den Wunsch gestellt hat, liest hier - und nur hier - das
 *              Ergebnis ab.
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
 * Schema-Nachzug von player_req_spawn_res_component.hpp. Der Validator nannte drei Maengel
 * (gemessen 2026-08-10, core/ase-validator/compliance/player/component/
 * player_req_spawn_res_component.hpp.cmplerr): das Legacy-Suffix `_component` statt `_comp`,
 * das ausgeschriebene `spawn` (die Taxonomie erlaubt hoechstens 4 Zeichen und fuehrt `spwn` in
 * DREI Kategorien - 03_act_action.json:14, 11_bio_biology.json:507, 29_ecs_ecs.json:32), und
 * `@category request`, das als Pfad nicht existiert: der gueltige Pfad heisst
 * `communication/request`. Weil der Strukturname aus dem Dateinamen folgt, wandert er mit:
 * PlayerReqSpawnResComponent wird PlayerReqSpwnResComponent. Die FELDER bleiben dieselben.
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
 * [ ] Filename: player_req_spwn_res_comp.hpp (module FULL, tax 3-4 chars)
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

#include <ase/ecs/system.hpp>

namespace ase::player {

/**
 * @brief PlayerReqSpwnResComponent - Bescheid auf einen Entstehungswunsch
 *
 * Der Bescheid liegt auf der WUNSCH-Entitaet, nicht auf der entstandenen Figur - sonst gaebe es
 * bei einem Fehlschlag niemanden, der ihn tragen koennte. `spawned_entity` steht deshalb auf
 * ecs::NullEntity und NICHT auf 0: die Null ist eine gueltige Entitaetsnummer, sie waere ein
 * Verweis auf irgendeine fremde Figur. Wer den Bescheid liest, prueft `success` zuerst.
 */
struct PlayerReqSpwnResComponent {
    ecs::Entity spawned_entity = ecs::NullEntity;  // entstandene Figur, sonst NullEntity
    bool success = false;                          // hat die Entstehung geklappt
};

}  // namespace ase::player
