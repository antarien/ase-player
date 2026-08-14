#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        player_mig_stg_pnd_tag.hpp
 * @brief       PlayerMigStgPndTag - migrate armed this pass, the PlayerSnap buffer still to be staged
 * @description Transient marker of PlayerMigSerSystem. Its walk filters on
 *              entt::exclude<PlayerBufMigComponent> and used to emplace that buffer from inside the
 *              walk - adding an excluded type drops the row out of the very set being iterated. The
 *              walk stamps this marker instead and a pass at the end of the tick encodes the snap.
 *
 *              TWO DIFFERENT JOBS AT THE SAME TYPE, do not merge them: the exclude is the re-entry
 *              barrier ACROSS ticks (it makes the epoch bump exactly once per armed migrate), this
 *              marker is the hand-over WITHIN one tick. The exclude therefore stays exactly as it
 *              was; only the emplace moved. Merging the two would let the second tick re-enter the
 *              walk and bump the epoch twice.
 *
 *              Nothing has to be carried out of the walk: player_ref and player_epoch live on
 *              PlayerStaEpchComponent, the position/yaw/velocity/status fields on their own
 *              components, and the request fields on PlayerReqMigComponent. snap_len is produced by
 *              the encode itself. (mig = migrate, stg = staging, pnd = pending, registered abbrevs.)
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    tag
 * @created     2026-08-14
 * @modified    2026-08-14
 * @version     1.0.0
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
 * [ ] Large data in registry.ctx() - N/A (Tags have no data)
 * [ ] Tag structs end with Tag suffix
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
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

struct PlayerMigStgPndTag {};

}  // namespace ase::player
