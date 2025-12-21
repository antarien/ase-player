#include <ase/player/systems/simulation/player_sim_phys_system.hpp>
#include <ase/player/components/state/player_state_pos_component.hpp>
#include <ase/player/components/state/player_state_vel_component.hpp>
#include <ase/player/components/state/player_state_phys_component.hpp>
#include <ase/player/components/state/player_state_cfg_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/log/log.hpp>

#include <cmath>

namespace ase::player {

void PlayerSimPhysSystem::on_start(ecs::Registry& /*registry*/) {
}

void PlayerSimPhysSystem::on_stop(ecs::Registry& /*registry*/) {
}

void PlayerSimPhysSystem::tick(ecs::Registry& registry, float dt) {
    HeightQueryFn height_query;
    MovementConfig config;
    auto config_view = registry.view<PlayerStateCfgComponent>();
    for (auto [config_entity, cfg] : config_view.each()) {
        height_query = cfg.height_query;
        config = cfg.movement;
        break;
    }

    auto view = registry.view<
        PlayerStatePosComponent,
        PlayerStateVelComponent,
        PlayerStatePhysComponent
    >();

    for (auto [entity, pos, vel, physics] : view.each()) {
        pos.x += vel.vx * dt;
        pos.y += vel.vy * dt;
        pos.z += vel.vz * dt;

        float ground_height = 0.0f;
        if (height_query) {
            ground_height = height_query(pos.x, pos.z);
        }

        if (pos.y <= ground_height + config.ground_snap_dist) {
            pos.y = ground_height;
            vel.vy = 0.0f;
            physics.on_ground = true;
        } else {
            physics.on_ground = false;
        }

        float vel_len = std::sqrt(vel.vx * vel.vx + vel.vz * vel.vz);
        if (vel_len > config.velocity_epsilon) {
            registry.emplace_or_replace<PlayerDirtyTag>(entity);
        }
    }
}

}  // namespace ase::player
