#include <ase/player/systems/spatial/player_spatial_chunk_system.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_chk_component.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>
#include <ase/player/components/tag/player_tag_chunk_changed_component.hpp>
#include <ase/player/types.hpp>
#include <ase/terrain/components/tag/terrain_tag_strm_obs_component.hpp>

#include <cmath>

namespace ase::player {

void PlayerSpatialChunkSystem::on_start(ecs::Registry& /*registry*/) {
}

void PlayerSpatialChunkSystem::on_stop(ecs::Registry& /*registry*/) {
}

void PlayerSpatialChunkSystem::tick(ecs::Registry& registry, float /*dt*/) {
    float chunk_size = MOVEMENT_DEFAULT_CHUNK_SIZE;

    auto mov_view = registry.view<PlayerStMovComponent>();
    for (auto [e, mov] : mov_view.each()) {
        chunk_size = mov.chunk_size;
        break;
    }

    auto view = registry.view<PlayerStPosComponent, PlayerStChkComponent>();

    for (auto [entity, pos, chunk] : view.each()) {
        int32_t new_chunk_x = static_cast<int32_t>(std::floor(pos.x / chunk_size));
        int32_t new_chunk_z = static_cast<int32_t>(std::floor(pos.z / chunk_size));

        if (new_chunk_x != chunk.chunk_x || new_chunk_z != chunk.chunk_y) {
            chunk.chunk_x = new_chunk_x;
            chunk.chunk_y = new_chunk_z;
            registry.emplace_or_replace<PlayerChunkChangedTag>(entity);

            auto* strm_obs = registry.try_get<terrain::TerrainStrmObsComponent>(entity);
            if (strm_obs) {
                strm_obs->chunk_x = new_chunk_x;
                strm_obs->chunk_y = new_chunk_z;
                strm_obs->needs_update = true;
            }
        }
    }
}

}  // namespace ase::player
