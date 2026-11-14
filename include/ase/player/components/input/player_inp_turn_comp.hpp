#pragma once

/**
 * ASE ECS COMPONENT (INPUT)
 *
 * @file        player_inp_turn_comp.hpp
 * @brief       One turn on the spot as the host filed it - the bridge between the hub row and the turn
 * @description Bridge component of the SYN pattern (PLAN_ASE_VIVARIUM_PHASE_07_FIGURE 07.7, fig.turn).
 *              PlayerHubTurnSystem reads one row HUB_PLR_TURN_REQ of ase-pl-avatar and lays it out on a
 *              request entity of its own, one entity per row (Entity-per-Item): the figure the turn is for
 *              and the yaw the host turned. PlayerCtrlTurnSystem adds it onto the open course of that
 *              figure (PlayerReqTurnComponent) and destroys the request entity.
 *
 * META
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    input
 * @parity      server_only
 * @usecase     usecase:101554 - receive player input
 * @actor       Player Input Warden (PLAYINPWAR)
 * @design      DSGN_021
 * @created     2026-11-14
 * @modified    2026-11-14
 * @version     1.0.0
 *
 * WHY server_only: the turn arrives through a host port of an embedding device; the web client turns no
 * figure through it.
 *
 * SYN PATTERN (Hub I/O vs Calculation Separation)
 *
 * This component is the BRIDGE between:
 *   - Sync System PlayerHubTurnSystem: reads the hub row, writes this component on a request entity
 *   - Calc System PlayerCtrlTurnSystem: reads this component, writes PlayerReqTurnComponent and
 *     PlayerStaYawComponent of the figure
 *
 * Hub Keys stored:
 *   HUB_PLR_TURN_FIGURE → figure (the figure the turn is for)
 *   HUB_PLR_TURN_YAW    → yaw    (radians the host turned)
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

#include <ase/ecs/system.hpp>

namespace ase::player {

/**
 * @brief One turn on the spot of a figure, as the host filed it (SERVER-ONLY bridge).
 *
 * figure stands on ecs::NullEntity and NOT on 0 until the sync system sets it: 0 is a valid entity number
 * and would name a figure nobody asked to turn.
 */
struct PlayerInpTurnComponent {
    ecs::Entity figure = ecs::NullEntity;  // Figure the turn is for (HUB_PLR_TURN_FIGURE)
    float yaw = 0.0f;                      // Radians the host turned (HUB_PLR_TURN_YAW)
};

}  // namespace ase::player
