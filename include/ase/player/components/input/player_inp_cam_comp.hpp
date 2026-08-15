#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT (INPUT)
 * =============================================================================
 *
 * @file        player_inp_cam_comp.hpp
 * @brief       External camera input data from Hub for player systems
 * @description Bridge component: Sync System writes Hub values here,
 *              Calc Systems read from here (SYN Pattern).
 *              Split out of PlayerInpExtComponent 2026-08-15: that component
 *              carried seven fields and exceeded the five-field limit. The cut
 *              follows the CONSUMPTION - these two are the only fields read by
 *              the camera path (PlayerCtrlInpSystem, PlayerCtrlMovSystem).
 *
 * -----------------------------------------------------------------------------
 * META
 * -----------------------------------------------------------------------------
 * @module      ase-player
 * @layer       3 (Module)
 * @category    input
 * @created     2026-08-15
 * @modified    2026-08-15
 * @version     1.0.0
 *
 * -----------------------------------------------------------------------------
 * SYN PATTERN (Hub I/O vs Calculation Separation)
 * -----------------------------------------------------------------------------
 * This component is the BRIDGE between:
 *   - Sync System (SERVER-ONLY): Reads Hub, writes this Component
 *   - Calc System (SHARED): Reads this Component, writes State Components
 *
 * Hub Keys stored:
 *   PLR_CAM_YAW    → cam_yaw    (Camera yaw radians)
 *   PLR_CAM_ORB    → cam_orb    (Orbit mode flag)
 *
 * -----------------------------------------------------------------------------
 * ECS COMPONENT COMPLIANCE
 * -----------------------------------------------------------------------------
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
 * [ ] Strings > 256 bytes use registry.ctx() mit Lookup-ID?
 * [ ] NO Entity-per-Character (strings are single attributes, not N-Items!)
 * [ ] Lookup-only strings use uint32_t hash (entt::hashed_string)
 * [ ] NO std::shared_ptr in components (use Flyweight Pattern via ctx!)
 * [ ] NO void* in components (use Flyweight Pattern via ctx!)
 * [ ] NO uint64_t as pointer concept (use uint32_t ID + ResourceManager via ctx!)
 * [ ] External library objects (shared_ptr, handles) in ResourceManager via ctx()
 * [ ] Component stores ONLY primitive ID (uint32_t) referencing external resource
 *
 * =============================================================================
 */

#include <cstdint>

namespace ase::player {

/**
 * @brief Camera input mirrored from the Hub for this player entity.
 *
 * Written once per tick by PlayerSyncInpSystem (Integration) from the owner-keyed
 * hub values PLR_CAM_YAW and PLR_CAM_ORB; read by the camera-facing calc systems.
 * Both fields are plain floats - the orbit flag rides as 0.0f/1.0f because the hub
 * value space is float32.
 */
struct PlayerInpCamComponent {
    float cam_yaw = 0.0f;     // Camera yaw (radians) from PLR_CAM_YAW
    float cam_orb = 0.0f;     // Orbit mode (0 or 1) from PLR_CAM_ORB
};

}  // namespace ase::player
