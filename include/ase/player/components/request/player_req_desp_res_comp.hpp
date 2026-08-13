#pragma once

/**
 * ASE ECS COMPONENT (REQUEST)
 *
 * @file        player_req_desp_res_comp.hpp
 * @brief       PlayerReqDespResComponent - the answer to one despawn request
 * @description Legt der Lebenszyklus auf DIESELBE Anfrage-Entitaet, damit der Aufrufer der
 *              Integration die Antwort dort findet, wo er die Bitte gestellt hat.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    communication/request
 * @parity      server_only
 * @created     2025-12-25
 * @modified    2026-08-10
 * @version     00.00.00.00000 [seed]
 *
 * DER UMZUG, NICHT DER UMBAU. Schema-Nachzug von player_req_desp_res_component.hpp: der Dateiname
 * trug die Altform `_component.hpp`, das Schema verlangt `_comp.hpp` (WRFL_ASE_NAMING_SCHEMA).
 * Der STRUKTURNAME war bereits korrekt und bleibt unveraendert - PlayerReqDespResComponent, ein
 * Feld, unveraendert. Dritte Einheit der Migration; die erste senkte den Stand gemessen von 221
 * auf 216 Verstoesse.
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
 * [ ] Filename: player_req_desp_res_comp.hpp (module FULL, tax 3-4 chars)
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
 * @brief PlayerReqDespResComponent - ob die Bitte ausgefuehrt wurde
 *
 * Ein einzelnes Ja oder Nein, und genau deshalb KEIN Tag: die Antwort muss auch dann lesbar sein,
 * wenn sie NEIN lautet. Ein Tag koennte nur das Ja ausdruecken, und sein Fehlen waere von "noch
 * nicht beantwortet" nicht zu unterscheiden.
 */
struct PlayerReqDespResComponent {
    bool success = false;  // true = Spieler wurde entfernt, false = nicht gefunden
};

}  // namespace ase::player
