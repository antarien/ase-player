#include <ase/player/systems/simulation/player_sim_phys_system.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_phys_component.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/types.hpp>
#include <ase/terrain/components/state/terrain_st_chk_crd_component.hpp>
#include <ase/terrain/components/state/terrain_st_chk_lyr_component.hpp>
#include <ase/terrain/types.hpp>

#include <cmath>

namespace ase::player {

namespace {

float get_terrain_height(ecs::Registry& registry, float world_x, float world_z) {
    int32_t chunk_x = static_cast<int32_t>(std::floor(world_x / terrain::CHUNK_SIZE));
    int32_t chunk_y = static_cast<int32_t>(std::floor(world_z / terrain::CHUNK_SIZE));

    auto view = registry.view<terrain::TerrainStChkCrdComponent, terrain::TerrainStChkLyrComponent>();
    for (auto [entity, crd, lyr] : view.each()) {
        if (crd.x == chunk_x && crd.y == chunk_y && lyr.hgt_ptr != 0) {
            float local_x = world_x - (chunk_x * terrain::CHUNK_SIZE);
            float local_z = world_z - (chunk_y * terrain::CHUNK_SIZE);
            size_t ix = static_cast<size_t>(local_x);
            size_t iy = static_cast<size_t>(local_z);
            if (ix < terrain::MACRO_RESOLUTION && iy < terrain::MACRO_RESOLUTION) {
                auto* hgt = reinterpret_cast<float*>(lyr.hgt_ptr);
                return hgt[iy * terrain::MACRO_RESOLUTION + ix];
            }
        }
    }
    return 0.0f;
}

}  // anonymous namespace

void PlayerSimPhysSystem::on_start(ecs::Registry& /*registry*/) {
}

void PlayerSimPhysSystem::on_stop(ecs::Registry& /*registry*/) {
}

void PlayerSimPhysSystem::tick(ecs::Registry& registry, float dt) {
    float ground_snap = MOVEMENT_DEFAULT_GROUND_SNAP_DIST;
    float vel_eps = MOVEMENT_DEFAULT_VELOCITY_EPSILON;

    auto mov_view = registry.view<PlayerStMovComponent>();
    for (auto [e, mov] : mov_view.each()) {
        ground_snap = mov.ground_snap_dist;
        vel_eps = mov.velocity_epsilon;
        break;
    }

    auto view = registry.view<
        PlayerStPosComponent,
        PlayerStVelComponent,
        PlayerStPhysComponent
    >();

    for (auto [entity, pos, vel, physics] : view.each()) {
        pos.x += vel.vx * dt;
        pos.y += vel.vy * dt;
        pos.z += vel.vz * dt;

        float ground_height = get_terrain_height(registry, pos.x, pos.z);

        if (pos.y <= ground_height + ground_snap) {
            pos.y = ground_height;
            vel.vy = 0.0f;
            physics.on_ground = true;
        } else {
            physics.on_ground = false;
        }

        float vel_len = std::sqrt(vel.vx * vel.vx + vel.vz * vel.vz);
        if (vel_len > vel_eps) {
            registry.emplace_or_replace<PlayerDirtyTag>(entity);
        }
    }
}

}  // namespace ase::player
