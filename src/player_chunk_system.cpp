#include <ase/player/systems/player_chunk_system.hpp>
#include <ase/player/components/player_position_component.hpp>
#include <ase/player/components/player_chunk_component.hpp>
#include <ase/player/components/player_config_component.hpp>
#include <ase/player/components/player_tags.hpp>
#include <ase/ecs/schedule_registry.hpp>
#include <ase/log/log.hpp>

#include <cmath>

namespace ase::player {

// =============================================================================
// LIFECYCLE
// =============================================================================

void PlayerChunkSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerChunkSystem] Started");
}

void PlayerChunkSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerChunkSystem] Stopped");
}

// =============================================================================
// TICK - Track player chunk presence
// =============================================================================

void PlayerChunkSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // Get config from singleton (SSOT for constants)
    MovementConfig config;
    auto config_view = registry.view<PlayerConfigComponent>();
    for (auto [config_entity, cfg] : config_view.each()) {
        config = cfg.movement;
        break;
    }

    auto view = registry.view<PlayerPositionComponent, PlayerChunkComponent>();

    for (auto [entity, pos, chunk] : view.each()) {
        int32_t new_chunk_x = static_cast<int32_t>(std::floor(pos.x / config.chunk_size));
        int32_t new_chunk_z = static_cast<int32_t>(std::floor(pos.z / config.chunk_size));

        if (new_chunk_x != chunk.chunk_x || new_chunk_z != chunk.chunk_y) {
            // Chunk changed
            chunk.chunk_x = new_chunk_x;
            chunk.chunk_y = new_chunk_z;

            // Mark for Neo4j LOCATED_IN update
            registry.emplace_or_replace<PlayerChunkChangedTag>(entity);
        }
    }
}

// =============================================================================
// REGISTRATION
// =============================================================================

REGISTER_SYSTEM(PlayerChunkSystem)
    .in_schedule(ecs::Schedule::FixedUpdate)
    .with_priority(74)
    .run_after("PlayerStateSystem");

}  // namespace ase::player
