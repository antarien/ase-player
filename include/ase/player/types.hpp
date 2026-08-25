#pragma once

/**
 * ASE MODULE TYPES (SSOT)
 *
 * @file        types.hpp
 * @brief       Single Source of Truth for ase-player constants and types
 * @description All compile-time constants, type aliases, and enumeration values.
 *              NO runtime values - those belong in Components!
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @created     2025-12-01
 * @modified    2026-01-22
 * @version     2.0.0
 *
 * ECS TYPES COMPLIANCE
 *
 * [ ] All constants defined (no magic numbers in code)
 * [ ] Every constant has inline comment (English, explains purpose)
 * [ ] NO enum class (only constexpr uint8_t for enumeration values)
 * [ ] Type aliases defined
 * [ ] InvalidEntityId = UINT32_MAX defined (if needed)
 * [ ] Abbreviations documented
 * [ ] NO structs (structs belong in Components)
 */

#include <cstdint>
#include <ase/types/types.hpp>

namespace ase::player {

/**
 * TYPE ALIASES
 * Custom type definitions for this module.
 */
using PlayerId = uint32_t;  // ID type for player entities
using TypId = uint16_t;     // Network serialization type identifier

/**
 * INVALID MARKERS
 * Sentinel values for uninitialized/invalid data.
 */
constexpr uint32_t InvalidPlayerId = 0;           // Invalid player ID sentinel
using ase::types::InvalidEntityId;                // SSOT: ase-types (Layer 0)

/**
 * SERIAL TYPE IDs (Layer 3: 1-999)
 * Network serialization type identifiers.
 */
constexpr TypId SERIAL_TYP_PLR_SPN = 1;  // Player spawn (id + pos)
constexpr TypId SERIAL_TYP_PLR_STA = 2;  // Player state blob (raw PlayerStaPosComponent bytes + id)

/**
 * BROADCAST CHANNELS
 * Network channel identifiers for player data.
 */
constexpr const char* CHANNEL_PLR_SPN = "player_spawn";  // Spawn broadcast channel
constexpr const char* CHANNEL_PLR_STA = "player_state";  // State broadcast channel
constexpr uint32_t CHANNEL_PLR_SPN_HASH = 0x706C7273;    // FNV-1a hash of "player_spawn"
constexpr uint32_t CHANNEL_PLR_STA_HASH = 0x706C7274;    // FNV-1a hash of "player_state"

/**
 * ENUMERATION VALUES - PLAYER STATE (NO enum class!)
 * Use with Tag-filtered Views, NOT switch/case!
 */
constexpr uint8_t PLAYER_STATE_IDLE = 0;      // Player is idle
constexpr uint8_t PLAYER_STATE_WALKING = 1;   // Player is walking
constexpr uint8_t PLAYER_STATE_RUNNING = 2;   // Player is running
constexpr uint8_t PLAYER_STATE_JUMPING = 3;   // Player is jumping
constexpr uint8_t PLAYER_STATE_FALLING = 4;   // Player is falling
constexpr uint8_t PLAYER_STATE_SWIMMING = 5;  // Player is swimming
constexpr uint8_t PLAYER_STATE_DEAD = 6;      // Player is dead

/**
 * DEFAULT VALUES - MOVEMENT SPEEDS
 * Fallback values for PlayerStMovComponent initialization.
 */
constexpr float MOVEMENT_DEFAULT_WALK_SPEED = 4.0f;     // Walk speed (m/s)
constexpr float MOVEMENT_DEFAULT_RUN_SPEED = 8.0f;      // Run speed (m/s)
constexpr float MOVEMENT_DEFAULT_JUMP_IMPULSE = 5.0f;   // Jump impulse (m/s)

/**
 * DEFAULT VALUES - PHYSICS
 * Physics simulation parameters.
 * NOTE: Gravity constant imported from ase-math PHYSICS_GRAVITY_EARTH_MS2
 */
constexpr float MOVEMENT_DEFAULT_GRAVITY = 9.8f;           // Gravity approximation (m/s^2)
constexpr float MOVEMENT_DEFAULT_GROUND_FRICTION = 1e1f;   // Ground friction coefficient
constexpr float MOVEMENT_DEFAULT_AIR_CONTROL = 0.3f;       // Air control factor (0-1)
constexpr float MOVEMENT_DEFAULT_GROUND_SNAP_DIST = 0.1f;  // Ground snap distance (m)

/**
 * DEFAULT VALUES - ROTATION
 * Rotation parameters.
 */
constexpr float MOVEMENT_DEFAULT_TURN_SPEED = 1e1f;  // Turn speed (rad/s)

/**
 * THE JOURNEY OF A BACKEND-DRIVEN PLAYER (PlayerSimWandSystem / PlayerSimRestSystem)
 *
 * A player the operator sets walking is a REAL ase-player entity that feeds the very same input
 * seam a human client feeds - PLR_INP_FWD and PLR_CAM_YAW. It never writes velocity and never
 * exceeds the movement authority; that is the difference to the cheat lever next door, which
 * exists precisely to break it.
 *
 * A JOURNEY IS NOT A LINE. The walk runs in LEGS: a leg holds one heading for a while, then the
 * walker rests, and only after the rest does a new heading get decided. Two reasons, both
 * mechanical rather than decorative. A straight line at constant speed leaves one furrow and can
 * never show a place being dwelt in. And the cell report downstream is dwell-gated
 * (ase-terrain TRN_CELL_EDGE_DWELL_TICKS): a walker who never lingers past the hysteresis band of
 * a freshly entered cell reports nothing at all, so the pauses are what makes the trail exist.
 *
 * The bounds below are the SPAN of a draw, never the draw itself: the leg and rest lengths of one
 * walker come from a deterministic mix of its own entity and its leg counter, so a world replays
 * identically and no two walkers march in step.
 */
constexpr float PLR_ROAM_LEG_MIN_SEC  = 20.0f;   // Shortest a single heading is held (s)
constexpr float PLR_ROAM_LEG_MAX_SEC  = 90.0f;   // Longest a single heading is held (s)
constexpr float PLR_ROAM_REST_MIN_SEC = 8.0f;    // Shortest pause between two legs (s)
constexpr float PLR_ROAM_REST_MAX_SEC = 40.0f;   // Longest pause between two legs (s)
constexpr float PLR_ROAM_TURN_MIN_RAD = 0.5236f; // Smallest course change at a leg change (30 deg)
constexpr float PLR_ROAM_TURN_MAX_RAD = 2.6180f; // Largest course change at a leg change (150 deg)

/**
 * THE DETERMINISTIC DRAW OF A WALKER
 *
 * No randomness in a system: the draw is an integer mix over (entity, leg counter, axis mixer),
 * the same shape ase-geoid uses for its spawn sequence. The resolution is the modulus the mixed
 * word is folded into before it becomes a 0..1 fraction.
 */
constexpr uint32_t PLR_ROAM_SEED            = 0x9E3779B9u;  // Golden-ratio odd word, the walk seed
constexpr uint32_t PLR_ROAM_MIX_LEG         = 0x85EBCA6Bu;  // Mixer of the leg-length axis
constexpr uint32_t PLR_ROAM_MIX_REST        = 0xC2B2AE35u;  // Mixer of the rest-length axis
constexpr uint32_t PLR_ROAM_MIX_TURN        = 0x27D4EB2Fu;  // Mixer of the course-change axis
constexpr uint32_t PLR_ROAM_MIX_SIDE        = 0x165667B1u;  // Mixer of the turn-direction axis
constexpr uint32_t PLR_ROAM_DRAW_RESOLUTION = 65536u;       // Fold modulus of one draw
constexpr float    PLR_ROAM_SIDE_SPLIT      = 0.5f;         // Draw below this turns left, above right

/**
 * A phase handover swaps two tags, and a tag swap during a walk over one of them would cut the
 * walk off. The handovers of one pass are therefore staged in a pass-local buffer and applied
 * after the View closes - the deferred shape ase-terrain uses for the very same reason. A walker
 * that does not fit in this pass keeps its phase one more tick and hands over then; nothing is
 * dropped, only postponed.
 */
constexpr uint32_t PLR_ROAM_DEFER_BATCH_MAX = 64u;  // Phase handovers applied per pass (bounded)

/**
 * The two readings an input key takes when it is not carrying a measured magnitude. They are
 * named because a bare 0 and a bare 1 in a hub write say nothing about which of the two they are.
 */
constexpr float PLR_ROAM_INPUT_STILL   = 0.0f;  // The key is not engaged this tick
constexpr float PLR_ROAM_INPUT_ENGAGED = 1.0f;  // The key is engaged this tick

/**
 * @brief One draw of a walker's deterministic sequence, as a 0..1 fraction
 * @param owner Entity of the walker - what makes two walkers of one tick differ
 * @param leg_index Legs begun so far - what makes two legs of one walker differ
 * @param mixer Axis mixer - what makes leg length, rest length, turn and side differ
 * @return Fraction in 0..1
 *
 * AND IT BELONGS IN THIS FILE, NOT IN A SYSTEM. Three systems draw from this sequence - the leg,
 * the rest and the birth of an errand. A copy in each anonymous namespace would be three
 * statements of one sequence, and the day one of them is touched the journeys stop replaying.
 */
constexpr float plr_roam_fraction(uint32_t owner, uint32_t leg_index, uint32_t mixer) {
    const uint32_t mixed = (owner ^ PLR_ROAM_SEED) + leg_index;
    const uint32_t spread = mixed * mixer;
    return static_cast<float>(spread % PLR_ROAM_DRAW_RESOLUTION) /
           static_cast<float>(PLR_ROAM_DRAW_RESOLUTION);
}

/**
 * @brief One draw placed inside a span - the only form in which a span is ever used
 * @param fraction Draw of plr_roam_fraction, 0..1
 * @param span_min Lower end of the span
 * @param span_max Upper end of the span
 * @return The drawn value inside the span
 */
constexpr float plr_roam_span(float fraction, float span_min, float span_max) {
    return span_min + fraction * (span_max - span_min);
}

/**
 * DEFAULT VALUES - THRESHOLDS
 * Movement threshold values.
 */
constexpr float MOVEMENT_DEFAULT_MIN_SPEED_THRESHOLD = 0.1f;  // Min speed threshold (m/s)
constexpr float MOVEMENT_DEFAULT_VELOCITY_EPSILON = 0.01f;    // Velocity epsilon (m/s)

/**
 * ANTI-CHEAT MOVEMENT AUTHORITY (Phase 13 / Task 13.10)
 * Engine-level deterministic bound on a player's realised horizontal speed. A legit player's
 * velocity is capped at run_speed by PlayerCtrlMovSystem, so a realised horizontal speed above
 * run_speed * margin cannot come from legitimate input — it is a speed-hack. The detector compares
 * the SQUARED horizontal speed (vx*vx + vz*vz) against the squared bound (no sqrt, frequency-
 * independent since velocity is m/s). A flagged player drives the contract trigger topic
 * "PLAYER_MOVEMENT_SUSPICIOUS" (hashed at the Hub call site), consumed by MovementValidator.
 */
constexpr float PLR_AC_SPEED_MARGIN = 1.5f;  // Tolerance factor over run_speed (slopes, knockback, latency)
constexpr float PLR_AC_MAX_HORIZONTAL_SPEED = MOVEMENT_DEFAULT_RUN_SPEED * PLR_AC_SPEED_MARGIN;  // Max plausible horizontal speed (m/s)
constexpr float PLR_AC_MAX_HORIZONTAL_SPEED_SQ = PLR_AC_MAX_HORIZONTAL_SPEED * PLR_AC_MAX_HORIZONTAL_SPEED;  // Squared bound vs vx*vx + vz*vz

/**
 * ANTI-CHEAT TELEPORT AUTHORITY (Phase 13 / Task 13.10)
 * A legit player moves at most run_speed*margin per second; at the Dynamics (30Hz) detector rate a single-
 * tick horizontal position delta is sub-metre. A per-tick delta exceeding PLR_AC_TELEPORT_STEP cannot be
 * legitimate motion — it is a teleport-hack. The detector compares the SQUARED per-tick position delta
 * (dx*dx + dz*dz) against the squared bound (no sqrt). The flagged player drives the SAME contract trigger
 * "PLAYER_MOVEMENT_SUSPICIOUS". PlayerStaAcpComponent holds the previous observed position per player.
 */
constexpr float PLR_AC_TELEPORT_STEP = PLR_AC_MAX_HORIZONTAL_SPEED;  // Max plausible single-tick horizontal position delta (m); larger = teleport
constexpr float PLR_AC_TELEPORT_STEP_SQ = PLR_AC_TELEPORT_STEP * PLR_AC_TELEPORT_STEP;  // Squared bound vs dx*dx + dz*dz

/**
 * ANTI-CHEAT ARCHETYPE AUTHORITIES (Phase 13 AP-3)
 * Engine-level deterministic bounds on the realised per-player activity RATES of the remaining
 * contract-topic families (ANTI_CHEAT_COMPONENT_CONTRACT.md): a realised rate above the bound cannot
 * come from legitimate play — human input, chat cadence and economy flow are all physically capped.
 * Each detector compares its realised rate component against ONE bound and drives ONE contract topic
 * (exact-hash literal, the `*` is PART of the topic name per the contract):
 *   PlayerStaActComponent.action_rate        > PLR_AC_MAX_ACTION_RATE       → "PLAYER_ACTION_*"        (BehaviorWatcher)
 *   PlayerStaCmbComponent.combat_rate        > PLR_AC_MAX_COMBAT_RATE       → "COMBAT_EVENT_*"         (BehaviorWatcher)
 *   PlayerStaComComponent.message_rate       > PLR_AC_MAX_MESSAGE_RATE      → "PLAYER_COMMUNICATION_*" (CoordinationDetector)
 *   PlayerStaEcoComponent.transaction_rate   > PLR_AC_MAX_TRANSACTION_RATE  → "ECONOMY_TRANSACTION_*"  (EconomyAuditor)
 *   PlayerStaInvComponent.modification_rate  > PLR_AC_MAX_INVENTORY_RATE    → "INVENTORY_MODIFICATION_*" (EconomyAuditor)
 * The realised rates are driven by the game simulation; the backend cheat levers (PLR_CHEAT_ACTIONS /
 * PLR_CHEAT_CHAT / PLR_CHEAT_DUPE, frame 76) induce them deterministically on the REAL player entity —
 * never a fabricated topic inject (same honesty rule as the speed lever).
 */
constexpr float PLR_AC_MAX_ACTION_RATE      = 20.0f;  // Max plausible player actions/sec (human input cap)
constexpr float PLR_AC_MAX_COMBAT_RATE      = 10.0f;  // Max plausible combat events/sec a player can cause
constexpr float PLR_AC_MAX_MESSAGE_RATE     = 5.0f;   // Max plausible chat messages/sec (human typing cap)
constexpr float PLR_AC_MAX_TRANSACTION_RATE = 8.0f;   // Max plausible economy transactions/sec
constexpr float PLR_AC_MAX_INVENTORY_RATE   = 15.0f;  // Max plausible inventory modifications/sec

/**
 * BACKEND-DRIVEN SPAWN WIRE (Phase 13 / Task 13.10)
 * The Replica forwards the ase-cli spawn frame VERBATIM to the World publisher; PlayerSpwnRcvSystem drains
 * transport::LANE_SPW and creates a PlayerReqSpwnComponent so PlayerLifeSpwnSystem spawns the real entity
 * (no human WebRTC client). Local mirror of ase-network BIN_MSG_PLAYER_SPAWN=77 (L3→L3 forbidden — the bytes
 * are restated here). Frame: [77][player_id:char[64]][x:f32][z:f32][speed:f32] = 77 bytes.
 *
 * THE WALKING SPEED RIDES IN THE SPAWN FRAME, not in a second lever afterwards. A player that is
 * spawned to walk has to be walking from its first tick, and a follow-up frame would leave a
 * window in which the entity stands somewhere without an errand - a state nobody asked for and
 * nobody could tell from a defect. A zero speed is the plain "spawn and stand" the coordinate
 * spawn always was, so the field extends the frame without changing what the old callers meant.
 */
constexpr uint8_t  PLR_BIN_MSG_PLAYER_SPAWN = 77;  // mirror of ase-network BIN_MSG_PLAYER_SPAWN (76 = PLAYER_CHEAT)
constexpr uint32_t PLR_SPW_FRAME_SZ         = 77u; // 1 type + 64 player_id + 4 x + 4 z + 4 speed
constexpr uint32_t PLR_SPW_PLAYER_ID_OFF    = 1u;  // player_id:char[64] offset (after the type byte)
constexpr uint32_t PLR_SPW_PLAYER_ID_LEN    = 64u; // player_id fixed field length (matches PlayerReqSpwnComponent.player_id[64])

/**
 * FNV-1a (64 Bit) fuer den Spieler-Index in PlayerLifeSpwnSystem.
 *
 * Der Index beantwortet "gibt es schon eine Zeile mit dieser player_id?" in O(1); vorher lief je
 * Anfrage eine volle view<PlayerStaIdntComponent>. Er ist ein VORFILTER: wer einen Treffer hat,
 * vergleicht die Kennung an der Fundstelle nach, weil eine Kollision sonst den FALSCHEN Spieler
 * liefern wuerde statt gar keinen.
 *
 * 64 Bit statt der 32 des Baums (rsn_skl_prvl_gate_sys.cpp): die 32-Bit-Fassung leitet
 * Hub-OWNER ab und ist dort festgelegt. Hier gibt es keine Hub-Bindung.
 */
constexpr uint64_t PLR_ID_FNV_OFFSET        = 14695981039346656037ull; // FNV-1a 64 offset basis
constexpr uint64_t PLR_ID_FNV_PRIME         = 1099511628211ull;        // FNV-1a 64 prime
constexpr uint32_t PLR_SPW_X_OFF            = 65u; // spawn x:f32 offset
constexpr uint32_t PLR_SPW_Z_OFF            = 69u; // spawn z:f32 offset
constexpr uint32_t PLR_SPW_SPEED_OFF        = 73u; // roam speed:f32 offset (0 = spawn and stand)
constexpr uint32_t PLR_SPW_RCV_BATCH_MAX    = 32u; // max spawn frames drained per tick (bounded)
constexpr float    PLR_SPW_SPEED_NONE       = 0.0f;// speed value that means "no errand, just stand"

/**
 * SESSION INDEX REGISTER (PlayerHubSessRegSystem)
 *
 * Hub v2.0 has no iteration API, so live players are addressed through registered index slots:
 * PLR_ACTIVE_COUNT (GLOBAL) says how many are live, the owner of slot i is the hash of
 * "PLR_ACTIVE_<i>", and PLR_OWNER read at that owner yields the player entity id. The slot bound
 * is a WIRE CONTRACT with the consumers - it must not exceed what they walk per pass
 * (ase-terrain TRN_OBS_SES_SLOT_MAX, terrain/types.hpp:208); a larger value here would publish
 * slots that no consumer ever reads. L3 modules may not include each other, so the bound is
 * restated rather than shared.
 */
constexpr uint32_t PLR_SESS_SLOT_MAX        = 256u; // max published PLR_ACTIVE_<i> index slots
constexpr uint32_t PLR_SESS_KEY_MAX         = 32u;  // char[N] capacity of a built "PLR_ACTIVE_<i>" key
constexpr uint32_t PLR_SESS_DIGIT_MAX       = 10u;  // decimal digits a uint32 slot index can carry

/**
 * DEFAULT VALUES - CAMERA/VIEW
 * Camera and view parameters.
 */
constexpr float MOVEMENT_DEFAULT_EYE_HEIGHT = 1.0f;  // Eye height (m)

/**
 * DEFAULT VALUES - CHUNK
 * Chunk size for spatial calculations.
 */
constexpr float MOVEMENT_DEFAULT_CHUNK_SIZE = 32.0f;  // Chunk size (m)

/**
 * MIGRATED TO hub_constants.json
 * The following constants are now in hub/data/hub_constants.json (SSOT):
 *   - LOG_CONST_DEFAULT_INTERVAL = 5.0 (was LOG_DEFAULT_INTERVAL)
 */

/**
 * ABBREVIATIONS (Documentation)
 * Used in filenames ONLY, between prefix and suffix.
 * Folder names are SPELLED OUT (state/, not sta/).
 *
 * Standard abbreviations used in this module (3-4 chars):
 *
 * │ Full Word │ Abbr │ Example                          │
 * │───────────│──────│──────────────────────────────────│
 * │ state     │ sta  │ player_sta_phys_comp.hpp         │
 * │ movement  │ mov  │ player_sta_inp_mov_comp.hpp      │
 * │ spawn     │ spwn │ player_life_spwn_sys.hpp         │
 * │ position  │ pos  │ player_sta_pos_comp.hpp          │
 * │ velocity  │ vel  │ player_sta_vel_comp.hpp          │
 * │ status    │ sts  │ player_sta_sts_comp.hpp          │
 *
 * NOTE: Folder names are SPELLED OUT (state/, not sta/)
 * NOTE: Abbreviations appear ONLY in filenames between prefix and suffix
 *
 * DIESE TABELLE WAR DIE FEHLERQUELLE, deshalb steht hier ihre Herkunft:
 * `state` stand jahrelang als `st` darin - zwei Zeichen, waehrend das Schema 3 bis 4 verlangt
 * (WRFL_ASE_NAMING_SCHEMA). Jede Datei, die sich an dieser Tabelle orientierte, erbte den
 * Verstoss. Verbindlich ist NICHT diese Tabelle, sondern
 * core/ase-validator/ecs_validator/data/taxonomy/_index.json (dort `{"abbrev": "sta", "name":
 * "state"}`) samt der Kategoriedateien darunter; die Tabelle hier ist eine Lesehilfe und wird
 * mit jedem Umzug nachgezogen.
 *
 * ZWEITER UMZUG, UND DER ERSTE HAT SEINE EIGENE AUSNAHME UEBERLEBT.
 *
 * Hier standen bis zuletzt zwei weitere Zeilen — `tag` und `dead` —, beide mit dem Beispielnamen
 * `player_tag_ded_component.hpp`, und darunter die ausdrueckliche Begruendung, sie traegen
 * "absichtlich den IST-Dateinamen, damit niemand einer Datei hinterherliest, die es noch nicht
 * gibt". Die Begruendung galt fuer DAMALS und ist heute widerlegt — gemessen ueber das WORT,
 * nicht ueber das Kuerzel der gestrichenen Zeile:
 *
 *     player_tag_ded_component.hpp   existiert im Modul NICHT
 *     `dead`                         der Katalog fuehrt das WORT als `dead`, und KEINE Datei
 *                                    des Moduls traegt diese Form
 *     `ded`                          steht im Katalog, bedeutet dort aber `dedicated`
 *                                    (06_str_structure.json:271). Die gestrichene Zeile nannte
 *                                    also keine unbekannte Form, sondern eine FREMDE — die
 *                                    gefaehrlichere Sorte, denn sie besteht jede Existenzpruefung
 *     `tag`                          der Katalog fuehrt `tag` und `tags`; im Modul kommt es
 *                                    ausschliesslich als SUFFIX vor, nie zwischen Praefix und
 *                                    Suffix — was diese Tabelle beschreibt. Gemessen ueber
 *                                    `_tag_` MITTIG; eine Suche nach `_tag` allein trifft jede
 *                                    Tag-Datei des Moduls und beantwortet die Frage nicht
 *
 * Beide Zeilen sind deshalb ENTFALLEN. Und der Vermerk faellt mit ihnen, weil er sonst eine
 * Ausnahme fuer Zeilen begruendet, die es nicht mehr gibt.
 *
 * WAS DARAN ZU LERNEN IST, und es ist der Grund fuer diesen Absatz: der erste Umzug hat das
 * Kuerzel gezogen (`st` wurde zu `sta`) und die zwei Zeilen mit dem ausgeschriebenen Suffix
 * stehen lassen — MIT einem Vermerk, der genau das rechtfertigte. Der Vermerk hat den
 * Rest-Defekt nicht dokumentiert, sondern KONSERVIERT: er nahm dem naechsten Leser den Anlass
 * nachzusehen, und kein Tor liest Prosa. Aufgefallen ist es erst, als eine Regel den
 * ausgeschriebenen Suffix selbst pruefte. Eine Begruendung im Kommentar ist Pruefgegenstand,
 * kein Freibrief.
 *
 * UND EIN ZWEITES, DAS ERST BEIM NACHMESSEN AUFFIEL: die Begruendung oben lautete zuerst
 * „`ded` als Taxonomieteil — keine Datei traegt es". Das ist die KUERZELrichtung. Sie fragt nach
 * einer Schreibweise und kann deshalb nur bestaetigen oder schweigen; welches Kuerzel der
 * Katalog fuer `dead` ueberhaupt fuehrt, hat sie nie gefragt. Das Ergebnis blieb dasselbe — zu
 * `dead` gibt es ebenfalls keine Datei —, also war die Begruendung ZUFAELLIG richtig.
 *
 * Und genau das ist der teurere Fall. Eine falsche Begruendung faellt auf, sobald jemand
 * nachrechnet; eine zufaellig richtige bestaetigt sich selbst, und der Naechste kopiert die
 * METHODE statt des Ergebnisses. In ase-replication ist derselbe Griff anders ausgegangen: dort
 * fiel eine Zeile als „keine Datei" heraus, weil nach `wip` gesucht wurde statt nach dem
 * katalogisierten `wipe` — und `replication_wipe_sys.hpp` stand die ganze Zeit da.
 */

}  // namespace ase::player
