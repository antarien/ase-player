#include <ase/player/systems/spatial/player_spatial_chunk_system.hpp>
#include <ase/player/components/state/player_state_pos_component.hpp>
#include <ase/player/components/state/player_state_chunk_component.hpp>
#include <ase/player/components/state/player_state_cfg_component.hpp>
#include <ase/player/components/tag/player_tag_chunk_changed_component.hpp>
#include <ase/log/log.hpp>

#include <cmath>

namespace ase::player {

void PlayerSpatialChunkSystem::on_start(ecs::Registry& /*registry*/) {
}

void PlayerSpatialChunkSystem::on_stop(ecs::Registry& /*registry*/) {
}

void PlayerSpatialChunkSystem::tick(ecs::Registry& registry, float /*dt*/) {
    MovementConfig config;
    auto config_view = registry.view<PlayerStateCfgComponent>();
    for (auto [config_entity, cfg] : config_view.each()) {
        config = cfg.movement;
        break;
    }

    auto view = registry.view<PlayerStatePosComponent, PlayerStateChunkComponent>();

    for (auto [entity, pos, chunk] : view.each()) {
        int32_t new_chunk_x = static_cast<int32_t>(std::floor(pos.x / config.chunk_size));
        int32_t new_chunk_z = static_cast<int32_t>(std::floor(pos.z / config.chunk_size));

        if (new_chunk_x != chunk.chunk_x || new_chunk_z != chunk.chunk_y) {
            chunk.chunk_x = new_chunk_x;
            chunk.chunk_y = new_chunk_z;
            registry.emplace_or_replace<PlayerChunkChangedTag>(entity);
        }
    }
}

}  // namespace ase::player
