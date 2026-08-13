#pragma once

/**
 * ASE ECS COMPONENT (REQUEST)
 *
 * @file        player_req_desp_comp.hpp
 * @brief       PlayerReqDespComponent - request to despawn a player entity
 * @description Raised by the Integration layer, drained by the player lifecycle pass.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    communication/request
 * @parity      server_only
 * @created     2025-12-25
 * @modified    2026-08-10
 * @version     00.00.00.00000 [seed]
 *
 * DER UMZUG, NICHT DER UMBAU. Diese Datei ist der Schema-Nachzug von
 * player_req_desp_component.hpp: der Dateiname trug die Altform `_component.hpp`, das Schema
 * verlangt `_comp.hpp` (WRFL_ASE_NAMING_SCHEMA). Der STRUKTURNAME war bereits korrekt und bleibt
 * unveraendert - PlayerReqDespComponent, Feld fuer Feld dasselbe. Es aendert sich genau ein Pfad,
 * und deshalb ist dies kein Umbau, dem man hinterherdebuggen muesste.
 *
 * Gemessen 2026-08-10: mod_cmpl_check --module ase-player meldete 221 Verstoesse in 32 Dateien,
 * durchweg Altnamen, waehrend 65 Dateien die aktuelle Form tragen - das Modul steht mitten in der
 * Migration. Diese Einheit hat die kleinste Reichweite von allen (fuenf Referenzen) und geht
 * deshalb zuerst. Die Kategorie heisst dabei communication/request und nicht request: der
 * Taxonomiepfad ist zweistufig, was die Altfassung noch nicht wusste.
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
 * [ ] Filename: player_req_desp_comp.hpp (module FULL, tax 3-4 chars)
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
 * @brief PlayerReqDespComponent - one pending despawn, named by the player it retires
 *
 * Der Spielername ist die Adresse der Bitte, nicht ein Etikett daneben: der Lebenszyklus loest
 * ihn zur Entitaet auf, weil der Aufrufer der Integration keine Entitaets-Kennung kennt.
 */
struct PlayerReqDespComponent {
    char player_id[64] = {};
};

}  // namespace ase::player
