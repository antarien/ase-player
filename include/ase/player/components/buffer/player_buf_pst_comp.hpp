#pragma once

/**
 * ASE ECS COMPONENT (BUFFER)
 *
 * @file        player_buf_pst_comp.hpp
 * @brief       PlayerBufPstComponent - was von dieser Figur auf dem Weg in die Ablage ist
 * @description Der Merkzettel einer laufenden Sicherung: unter welchen Blob-Kennungen der
 *              erzeugte Text und die Kennung der Figur beim Sicherungsverwalter liegen, wie lang
 *              beide sind, und wie weit die Sicherung gediehen ist.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    persistence/serialization
 * @parity      server_only
 * @created     2025-12-25
 * @modified    2026-08-10
 * @version     00.00.00.00000 [seed]
 *
 * UMZUG MIT INHALTLICHER KORREKTUR: HIER STANDEN ZWEI ROHE ADRESSEN.
 *
 * Schema-Nachzug von player_buf_pst_component.hpp. Der Validator nannte drei Arten von Maengeln
 * (gemessen 2026-08-10, core/ase-validator/compliance/player/component/
 * player_buf_pst_component.hpp.cmplerr):
 *   1. Legacy-Suffix `_component` statt `_comp`.
 *   2. `@category buffer` ist kein gueltiger Taxonomiepfad. Der Validator schlug `gis/query/buffer`
 *      vor - das ist der Pufferring einer Geoabfrage und hat mit einer Sicherung NICHTS zu tun.
 *      Richtig ist `persistence/serialization` (taxonomy/10_pst_persistence.json:3 und :121);
 *      genau diesen Pfad fuehrt auch core/ase-serial/.../serial_jsn_resource_manager.hpp.
 *      Ein Validator-Vorschlag ist ein Vorschlag, kein Befehl.
 *   3. ZWEIMAL `uint64_t` als Zeigerbegriff - der eigentliche Fehler. Eine Komponente darf keine
 *      Adresse tragen: sie ueberlebt keinen Modulwechsel, keine Serialisierung und keinen
 *      Prozess. An ihre Stelle tritt die Blob-Kennung des Sicherungsverwalters
 *      (serial::SerialJsnResourceManager::store_blob, get_blob, get_blob_len), der in
 *      registry.ctx() lebt und Adresse plus Laenge zusammen fuehrt. Das ist KEIN neuer
 *      Mechanismus: derselbe Verwalter benennt in diesem System bereits die Quelldaten des
 *      Textes (player_pst_ser_sys.cpp:224).
 *
 * OFFENER VERBRAUCHER, ABSICHTLICH STEHENGELASSEN: `jsn_ref`/`jsn_len` werden derzeit von
 * niemandem geschrieben, und den Merkzettel liest bislang niemand aus (gemessen 2026-08-10, der
 * einzige Zugriff steht in player_pst_ser_sys.cpp:243). Ein leeres Feld heisst FEHLENDER
 * ERZEUGER, nicht totes Feld - die Sicherungsseite in ase-replication holt den Text heute
 * direkt ueber get_json(Anfrage-Entitaet) und laesst diesen Merkzettel links liegen. Wer die
 * Zusammenfuehrung baut, findet hier den vorbereiteten Platz.
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
 * [ ] Filename: player_buf_pst_comp.hpp (module FULL, tax 3-4 chars)
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
 * @brief PlayerBufPstComponent - Merkzettel einer laufenden Sicherung
 *
 * Die beiden Kennungen sind LOOKUP-Werte, keine Adressen: aufgeloest werden sie ueber
 * registry.ctx().get<serial::SerialJsnResourceManager&>().get_blob(ref). Der Verwalter fuehrt zu
 * jeder Kennung Adresse UND Laenge; die Laengenfelder hier sind die vom Erzeuger gemessene
 * Kopie, damit ein Leser, der nur die Groesse braucht, den Verwalter nicht bemuehen muss. Beide
 * werden im selben Zug gesetzt (player_pst_ser_sys.cpp) und koennen deshalb nicht auseinander
 * laufen.
 *
 * Die Null ist bei beiden Kennungen ungueltig (serial::SERIAL_INVALID_BLB_ID) - ein Merkzettel
 * mit Null bedeutet also "noch nichts hinterlegt", nicht "Blob Nummer null".
 */
struct PlayerBufPstComponent {
    uint32_t jsn_ref = 0;     // Blob-Kennung des erzeugten Sicherungstextes (0 = keine)
    uint32_t jsn_len = 0;     // Laenge dieses Textes in Bytes
    uint32_t plr_id_ref = 0;  // Blob-Kennung der Figurenkennung (0 = keine)
    uint32_t plr_id_len = 0;  // Laenge der Figurenkennung in Bytes, ohne Nullzeichen
    uint8_t st = 0;           // Fortschritt der Sicherung (siehe PLAYER_PST_ST_* in types.hpp)
};

}  // namespace ase::player
