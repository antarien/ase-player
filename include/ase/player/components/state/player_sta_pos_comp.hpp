#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        player_sta_pos_comp.hpp
 * @brief       PlayerStaPosComponent - wo die Figur steht, chunk-relativ
 * @description Der Ort einer Spielerfigur als Wabenadresse plus Meter in der Wabe. Aus diesem
 *              Wert leiten sich die Wabe ab, in der sie sich aufhaelt, die
 *              Regionszustaendigkeit, der Gelaendestrom um sie herum und jede Farbspur, die
 *              ihr Weg im Geoid hinterlaesst.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    state
 * @parity      server_only
 * @created     2025-12-25
 * @modified    2026-08-11
 * @version     00.00.00.00000 [seed]
 *
 * CHUNK-RELATIV SEIT 2026-08-11 (S2b, Spiegelschnitt zu SpatialIdxPosComponent; Registerlage:
 * TASKREGISTER:521 "Positionen chunk-relativ fuehren" und TASKREGISTER:1724 "PlayerSnap v2 ...
 * (chunk:i32, local:f32)" - der DRAHT selbst bleibt hier unangetastet, siehe unten).
 *
 * Die absolute f32-Weltmeterachse traegt den Adressraum nicht: LIVE GEMESSEN 2026-08-11 14:09
 * wurde siedler_7 an Ort 7 bei x=204144660 Weltmetern gespawnt - dort ist die float32-ULP
 * 16 Meter, und jeder 0.04-m-Schritt der Physik (player_sim_phys_sys STEP 3, pos.x += vel*dt)
 * verpufft. Die fuenf Ort-Siedler standen still, erzeugten keine Uebertritte und keine Spur.
 * Dieselbe Falle hat P22 in ase-spatial geschnitten (spatial_idx_pos_comp.hpp, Messung
 * 03-05 Uhr: 4352 Wegstueck-Uhren, null bewegte Koerper). Die Loesung ist dieselbe: die
 * Wabenadresse als int32 EXAKT (traegt die Face-Kodierung verlustfrei), die Meter in der Wabe
 * als kleiner float mit voller Praezision.
 *
 * DER GIERWINKEL IST UMGEZOGEN: Blick ist nicht Ort (5-Felder-Grenze, Praezedenz "extent is
 * not location" aus dem P22-Schnitt) - er wohnt jetzt in PlayerStaYawComponent.
 *
 * DER DRAHT BLEIBT UNANGETASTET: der eingefrorene PlayerSnap (ase/types/region_wire.hpp,
 * 4x f32) traegt weiterhin Weltmeter; Serialisierer bilden die Summe
 * chunk * Wabenkante + local, Empfaenger zerlegen per floor/Rest (Muster
 * character_life_spwn_sys.cpp). Fuer Wabenkanten-Summen sind f32-Meter exakt (Vielfache der
 * Kante unterhalb 2^28 liegen auf Mantissenbits). Das Wire-v2 aus TASKREGISTER:1724 ist
 * Entscheidungsklasse des Betreibers und NICHT Teil dieses Schnitts.
 *
 * ACHSENFALLE, GEMESSEN 2026-08-10: in DIESEM Modul ist Y die Senkrechte - die Physik schreibt
 * y aus der Fallgeschwindigkeit, und das Wabenraster spannen X und Z auf. Auf der Geoid-/
 * GIS-Seite ist dagegen Z die Hoehenachse. Wer Werte zwischen beiden Welten traegt, TAUSCHT
 * also - er kopiert nicht.
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
 * [ ] Filename: player_sta_pos_comp.hpp (module FULL, tax 3-4 chars)
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
 * @brief PlayerStaPosComponent - Ort dieses Taktes, chunk-relativ
 *
 * Die Wabenadresse ist exakt (int32), die Meter in der Wabe sind klein und tragen volle
 * float-Praezision - ein Schritt verpufft nie, egal wo auf dem Planeten die Figur steht.
 */
struct PlayerStaPosComponent {
    int32_t chunk_x = 0;    // Wabenadresse X (exakt, traegt die Face-Kodierung verlustfrei)
    int32_t chunk_z = 0;    // Wabenadresse Z (exakt)
    float local_x = 0.0f;   // Meter in der Wabe X, [0, Wabenkante)
    float local_z = 0.0f;   // Meter in der Wabe Z, [0, Wabenkante)
    float y = 0.0f;         // Weltmeter entlang der Senkrechten (Achse unveraendert)
};

}  // namespace ase::player
