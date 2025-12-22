#include <ase/player/systems/log/player_log_causality_system.hpp>
#include <ase/player/components/cache/player_cache_obs_component.hpp>
#include <ase/player/types.hpp>
#include <ase/player/components/tag/player_tag_mgr_component.hpp>
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/components/tag/player_tag_chunk_changed_component.hpp>
#include <ase/player/types.hpp>
#include <ase/log/log.hpp>

namespace ase::player {

void PlayerLogCausalitySystem::on_start(ecs::Registry& /*registry*/) {
    // Cache component is added on first tick if needed
}

void PlayerLogCausalitySystem::tick(ecs::Registry& registry, float dt) {
    // Find player manager
    auto mgr_view = registry.view<PlayerMgrTag>();
    for (auto mgr : mgr_view) {
        auto& cache = registry.get_or_emplace<PlayerCacheObsComponent>(mgr);

        // Update timer
        cache.log_interval_timer += dt;

        // Count players
        uint32_t player_count = 0;
        auto player_view = registry.view<PlayerStIdComponent>();
        for (auto e : player_view) { (void)e; ++player_count; }

        // Count moving players (not idle)
        uint32_t moving_count = 0;
        auto status_view = registry.view<PlayerStStsComponent>();
        for (auto [e, status] : status_view.each()) {
            if (status.state != PLAYER_STATE_IDLE) {
                ++moving_count;
            }
        }

        // Detect significant change
        bool significant_change =
            player_count != cache.last_player_count ||
            moving_count != cache.last_moving_count;

        // Log on interval or significant change
        if (cache.log_interval_timer >= LOG_DEFAULT_INTERVAL || significant_change) {
            cache.log_interval_timer = 0.0f;
            cache.last_player_count = player_count;
            cache.last_moving_count = moving_count;

            // Count by state
            uint32_t idle_count = 0;
            uint32_t walking_count = 0;
            uint32_t running_count = 0;
            uint32_t jumping_count = 0;
            uint32_t falling_count = 0;

            for (auto [e, status] : status_view.each()) {
                switch (status.state) {
                    case PLAYER_STATE_IDLE:    ++idle_count; break;
                    case PLAYER_STATE_WALKING: ++walking_count; break;
                    case PLAYER_STATE_RUNNING: ++running_count; break;
                    case PLAYER_STATE_JUMPING: ++jumping_count; break;
                    case PLAYER_STATE_FALLING: ++falling_count; break;
                    default: break;
                }
            }

            // Count tags
            uint32_t spawned_count = 0;
            auto spawned_view = registry.view<PlayerSpawnedTag>();
            for (auto e : spawned_view) { (void)e; ++spawned_count; }

            uint32_t dirty_count = 0;
            auto dirty_view = registry.view<PlayerDirtyTag>();
            for (auto e : dirty_view) { (void)e; ++dirty_count; }

            uint32_t chunk_changed_count = 0;
            auto chunk_view = registry.view<PlayerChunkChangedTag>();
            for (auto e : chunk_view) { (void)e; ++chunk_changed_count; }

            log::debug("\x1b[38;5;141m[ase-player]\x1b[0m [PlayerLogCausalitySystem] "
                       "players:{} → idle:{} → walk:{} → run:{} → jump:{} → fall:{} → "
                       "dirty:{} → spawned:{} → chk_chg:{}",
                       player_count, idle_count, walking_count, running_count,
                       jumping_count, falling_count, dirty_count, spawned_count,
                       chunk_changed_count);
        }
    }
}

void PlayerLogCausalitySystem::on_stop(ecs::Registry& /*registry*/) {
    // No cleanup needed
}

}  // namespace ase::player
