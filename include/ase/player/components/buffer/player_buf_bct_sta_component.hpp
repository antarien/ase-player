#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT
 * =============================================================================
 *
 * @file        player_buf_bct_sta_component.hpp
 * @brief       Buffer for state broadcast serialization
 * @description Combines Id + Pos + Vel + Sts components for network
 *
 * -----------------------------------------------------------------------------
 * META
 * -----------------------------------------------------------------------------
 * @module      ase-player
 * @layer       3 (Module)
 * @category    buffer
 * @created     2025-12-25
 * @modified    2025-12-25
 * @version     1.0.0
 * @author      Jan Ohlmann (ADG/ASE/AOW)
 *
 * -----------------------------------------------------------------------------
 * ECS COMPONENT COMPLIANCE (15 Checkpoints)
 * -----------------------------------------------------------------------------
 * [x] DATA fields ONLY - No methods
 * [x] NO .cpp file - Header-only
 * [x] ONLY zero-initialization (= 0, = 0.0f, = false, = {})
 * [x] No magic numbers in defaults
 * [x] Entity references use InvalidEntityId (not 0)
 * [x] Single responsibility (one data category)
 * [x] No God-Component (unrelated fields)
 * [x] Large data uses pointer pattern (uint64_t ptr = 0)
 * [x] Tag structs end with Tag suffix
 * [x] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [x] Struct name derived from filename (snake_case to PascalCase)
 * [x] 1 File = 1 Component
 * [x] Folder in components/[category]/
 * [x] SHARED components listed in codegen.json components.shared
 * [x] Pointer components in codegen.json components.server_only
 *
 * =============================================================================
 */

#include <array>
#include <cstdint>

namespace ase::player {

struct PlayerBufBctStaComponent {
    std::array<char, 64> player_id = {};
    uint64_t spawned_at_ms = 0;
    uint64_t last_input_ms = 0;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float yaw = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float vz = 0.0f;
    uint8_t state = 0;
};

}  // namespace ase::player
