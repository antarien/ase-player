#pragma once

/**
 * ASE ECS COMPONENT (INPUT)
 *
 * @file        player_inp_mig_dlvr_comp.hpp
 * @brief       PlayerInpMigDlvrComponent - one inbound migrate image, mirrored from the Hub
 * @description Bridge component: PlayerMigDesSystem copies one hub row HUB_PLR_MIG_DLV into it
 *              (the frozen PlayerSnap and the region the Replica resolved as the new owner), the
 *              calc system PlayerMigAplySystem reads it and builds or updates the figure (SYN
 *              Pattern). One bridge entity per delivery; PlayerMigAplySystem destroys it after
 *              the apply. The image stays exactly as it came off frame 100: length and schema
 *              are checked by the calc system, the delivery is checked, not believed.
 *
 *              mig = migrate, dlvr = delivery (taxonomy catalogue)
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    input
 * @parity      server_only
 * @design      DSGN_021
 * @usecase     usecase:100692 - migrate figure between regions
 * @created     2026-10-14
 * @modified    2026-10-14
 * @version     1.0.0
 *
 * SYN PATTERN (Hub I/O vs Calculation Separation)
 *
 * This component is the BRIDGE between:
 *   - Sync System (SERVER-ONLY): PlayerMigDesSystem reads the Hub row, writes this Component
 *   - Calc System: PlayerMigAplySystem reads this Component, writes the figure's State Components
 *
 * Server only, because the migrate is a hand-over between two World nodes: frame 100 never
 * reaches a browser, so a client has no source for this input.
 *
 * Hub row mirrored:
 *   HUB_PLR_MIG_DLV_SNAP       → snap, snap_len
 *   HUB_PLR_MIG_DLV_DST_REGION → dst_region
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
 * [ ] Strings > 256 bytes use registry.ctx() mit Lookup-ID?
 * [ ] NO Entity-per-Character (strings are single attributes, not N-Items!)
 * [ ] Lookup-only strings use uint32_t hash (entt::hashed_string)
 * [ ] NO std::shared_ptr in components (use Flyweight Pattern via ctx!)
 * [ ] NO void* in components (use Flyweight Pattern via ctx!)
 * [ ] NO uint64_t as pointer concept (use uint32_t ID + ResourceManager via ctx!)
 * [ ] External library objects (shared_ptr, handles) in ResourceManager via ctx()
 * [ ] Component stores ONLY primitive ID (uint32_t) referencing external resource
 */

#include <ase/types/region_wire.hpp>

#include <cstdint>

namespace ase::player {

/**
 * @brief One inbound migrate image and the region that now owns the figure.
 *
 * Written by PlayerMigDesSystem (Reception) from the hub row HUB_PLR_MIG_DLV; read and destroyed
 * by PlayerMigAplySystem (Reception, run_after PlayerMigDesSystem).
 */
struct PlayerInpMigDlvrComponent {
    char     snap[types::PLAYER_SNAP_SZ] = {};  // frozen PlayerSnap bytes, single attribute
    uint32_t snap_len   = 0;                    // bytes the row held; the calc checks it against the contract size
    uint32_t dst_region = 0;                    // region of this node the Replica resolved as the owner
};

}  // namespace ase::player
