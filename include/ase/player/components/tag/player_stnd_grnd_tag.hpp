#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_stnd_grnd_tag.hpp
 * @brief       PlayerStndGrndTag - ase-physics says the walking figure stands on its ground
 * @description Input tag of a walking figure: the mark HUB_PHYS_STAND_GRND of the answer of ase-physics,
 *              laid by PlayerHubStndSyncSystem while the mark stands. PlayerSimStndSystem reads it into
 *              PlayerStaPhysComponent.on_ground, which the steering and the gait read.
 *
 * @module      ase-player
 * @layer       3 (Modules)
 * @category    tag
 * @parity      shared
 * @design      DSGN_030
 * @usecase     usecase:101588 - move player figure over ground
 * @actor       SS/PC
 * @created     2026-11-14
 * @modified    2026-11-14
 * @version     1.0.0
 *
 * KUERZEL AUS DER TAXONOMIE: "stand": "stnd" (03_act_action), "ground": "grnd", nachgeschlagen.
 *
 * ECS TAG COMPLIANCE
 *
 * [ ] DATA fields ONLY - No methods (empty struct for tags)
 * [ ] NO .cpp file - Header-only
 * [ ] ONLY zero-initialization - N/A (no fields)
 * [ ] No magic numbers in defaults - N/A (no fields)
 * [ ] Entity references - N/A (no fields)
 * [ ] Single responsibility - N/A (marker only)
 * [ ] No God-Component - N/A (no fields)
 * [ ] Large data uses pointer pattern - N/A (no data)
 * [ ] Large data in registry.ctx() - N/A (Tags have no data)
 * [ ] Tag structs end with Tag suffix
 * [ ] Filename: player_stnd_grnd_tag.hpp (module FULL, tax 3-4 chars)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: PlayerStndGrndTag (PascalCase)
 * [ ] Struct name: Remove tag_ from middle, add Tag suffix
 * [ ] 1 File = 1 Component
 * [ ] File in tag/ subfolder (with optional deeper hierarchy)
 * [ ] Per-entity runtime values use state/ components (NOT types.hpp!)
 * [ ] SHARED components listed in codegen.json components.shared
 * [ ] Pointer components in codegen.json components.server_only
 * [ ] Tag replaces `bool is_*` or `bool has_*` field in Component
 * [ ] Tag replaces `uint8_t *_type` field with if-chain dispatch
 * [ ] Systems use View filter instead of if-else inside loop
 * [ ] INCLUDE: registry.view<Component, ThisTag>()
 * [ ] EXCLUDE: registry.view<Component>(entt::exclude<ThisTag>)
 * [ ] NO if (entity.has<Tag>) inside loop - use filtered View!
 * [ ] NO switch/case on type - use separate View per Tag!
 * [ ] Each state = separate Tag + separate View in System
 * [ ] N-item support via Entity-per-Item + Tags
 */

namespace ase::player {

/**
 * @brief PlayerStndGrndTag - ase-physics says the walking figure stands on its ground
 *
 * State: the answer HUB_PHYS_STAND carries its mark HUB_PHYS_STAND_GRND
 * Filter: PlayerSimStndSystem (on_ground true)
 * Added: PlayerHubStndSyncSystem, while the mark stands
 * Removed: PlayerHubStndSyncSystem, when the mark comes off or the answer is withdrawn
 */
struct PlayerStndGrndTag {};

}  // namespace ase::player
