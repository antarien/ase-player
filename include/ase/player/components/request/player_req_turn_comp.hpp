#pragma once

/**
 * ASE ECS COMPONENT
 *
 * @file        player_req_turn_comp.hpp
 * @brief       The open course of a turn on the spot - what the figure has yet to turn
 * @description SERVER-ONLY request on the figure itself (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.7,
 *              fig.turn). PlayerHubTurnSystem lays every turn row HUB_PLR_TURN_REQ of ase-pl-avatar out as
 *              PlayerInpTurnComponent; PlayerCtrlTurnSystem adds each of them onto it, turns
 *              PlayerStaYawComponent by at most MOVEMENT_DEFAULT_TURN_SPEED per second out of it and
 *              removes it once nothing is left. Its presence is the request: a figure without it has no
 *              turn open.
 *
 * META
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @design      DSGN_030
 * @category    communication/request
 * @parity      server_only
 * @created     2026-11-14
 * @modified    2026-11-14
 * @version     1.0.0
 * @author      Jan Ohlmann (ADG/ASE/AOW)
 *
 * WHY server_only: the turn arrives through a host port of an embedding device; the web client turns no
 * figure through it and reads the heading from PlayerStaYawComponent.
 *
 * A TURN AND NOT A PLACE: unlike the spawn request (PlayerReqSpwnComponent, which names x and z and leaves
 * the height to the ground) this request names no axis of the world at all, only an angle about the up
 * axis of the figure. The place of the figure stays with ase-physics.
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
 * [ ] Large data in registry.ctx()? (component has only lookup ID!)
 * [ ] Tag structs end with Tag suffix - N/A (not a tag)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name derived from filename (snake_case to PascalCase)
 * [ ] 1 File = 1 Component
 * [ ] File in correct category subfolder
 * [ ] SHARED components listed in codegen.json components.shared
 * [ ] Pointer components in codegen.json components.server_only
 * [ ] Strings < 64 bytes use char[N] fixed arrays
 * [ ] Strings 64-256 bytes use appropriately sized char[N]
 * [ ] Strings > 256 bytes use registry.ctx() with lookup ID?
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
 * @brief The open course of a turn on the spot (SERVER-ONLY).
 *
 * open_yaw is radians, signed like PlayerStaYawComponent: what the host asked to turn and the figure has
 * not turned yet. Present only while a turn is open.
 */
struct PlayerReqTurnComponent {
    float open_yaw = 0.0f;  // Turn still to make (rad), positive the way the yaw grows
};

}  // namespace ase::player
