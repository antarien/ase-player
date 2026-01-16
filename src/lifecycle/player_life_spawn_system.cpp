#include <ase/player/systems/lifecycle/player_life_spawn_system.hpp>
#include <ase/player/components/request/player_req_spawn_component.hpp>
#include <ase/player/components/request/player_req_desp_component.hpp>
#include <ase/player/components/request/player_req_spawn_res_component.hpp>
#include <ase/player/components/request/player_req_desp_res_component.hpp>
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_phys_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/state/player_st_chk_component.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/components/tag/player_tag_mgr_component.hpp>
#include <ase/player/types.hpp>
#include <ase/input/input.hpp>
#include <ase/camera/camera.hpp>
#include <ase/terrain/components/tag/terrain_tag_strm_obs_component.hpp>
#include <ase/terrain/components/state/terrain_st_chk_crd_component.hpp>
#include <ase/terrain/components/state/terrain_st_chk_lyr_component.hpp>
#include <ase/terrain/types.hpp>
#include <ase/network/components/player/req/network_plr_req_spawn_component.hpp>
#include <ase/network/components/player/req/network_plr_req_despawn_component.hpp>
#include <ase/network/components/player/state/network_plr_state_input_component.hpp>
#include <ase/log/log.hpp>

#include <cmath>
#include <vector>

namespace ase::player {

namespace {

// OOP ANTI-PATTERN REMOVED:
// find_player_by_id(), get_terrain_height(), get_movement_settings()
// helpers with View iteration are FORBIDDEN!
// See INST_ASE_ECS_SER_TAGS.md
//
// INLINED: View iteration now happens directly in calling functions.
// Views created once per function, then iterated inline.

ecs::Entity create_player_entity(
    ecs::Registry& registry,
    const std::string& player_id,
    float x, float z
) {
    // INLINED: get_terrain_height() - no helper with View!
    float ground_y = 0.0f;
    {
        int32_t chunk_x = static_cast<int32_t>(std::floor(x / terrain::CHUNK_SIZE));
        int32_t chunk_y = static_cast<int32_t>(std::floor(z / terrain::CHUNK_SIZE));
        auto terrain_view = registry.view<terrain::TerrainStChkCrdComponent, terrain::TerrainStChkLyrComponent>();
        for (auto [te, crd, lyr] : terrain_view.each()) {
            if (crd.x == chunk_x && crd.y == chunk_y && lyr.hgt_ptr != 0) {
                float local_x = x - (chunk_x * terrain::CHUNK_SIZE);
                float local_z = z - (chunk_y * terrain::CHUNK_SIZE);
                size_t ix = static_cast<size_t>(local_x);
                size_t iy = static_cast<size_t>(local_z);
                if (ix < terrain::MACRO_RESOLUTION && iy < terrain::MACRO_RESOLUTION) {
                    auto* hgt = reinterpret_cast<float*>(lyr.hgt_ptr);
                    ground_y = hgt[iy * terrain::MACRO_RESOLUTION + ix];
                }
                break;  // Found the chunk
            }
        }
    }

    // INLINED: get_movement_settings() - no helper with View!
    PlayerStMovComponent mov;
    mov.walk_speed = MOVEMENT_DEFAULT_WALK_SPEED;
    mov.run_speed = MOVEMENT_DEFAULT_RUN_SPEED;
    mov.jump_impulse = MOVEMENT_DEFAULT_JUMP_IMPULSE;
    mov.gravity = MOVEMENT_DEFAULT_GRAVITY;
    mov.ground_friction = MOVEMENT_DEFAULT_GROUND_FRICTION;
    mov.air_control = MOVEMENT_DEFAULT_AIR_CONTROL;
    mov.ground_snap_dist = MOVEMENT_DEFAULT_GROUND_SNAP_DIST;
    mov.turn_speed = MOVEMENT_DEFAULT_TURN_SPEED;
    mov.min_speed_threshold = MOVEMENT_DEFAULT_MIN_SPEED_THRESHOLD;
    mov.velocity_epsilon = MOVEMENT_DEFAULT_VELOCITY_EPSILON;
    mov.eye_height = MOVEMENT_DEFAULT_EYE_HEIGHT;
    mov.chunk_size = MOVEMENT_DEFAULT_CHUNK_SIZE;
    // Override with existing settings if any player has them
    {
        auto mov_view = registry.view<PlayerStMovComponent>();
        for (auto [me, existing_mov] : mov_view.each()) {
            mov = existing_mov;
            break;  // Use first found
        }
    }

    auto entity = registry.create();

    auto& identity = registry.emplace<PlayerStIdComponent>(entity);
    identity.player_id = player_id;
    identity.spawned_at = std::chrono::steady_clock::now();
    identity.last_input = identity.spawned_at;

    auto& pos = registry.emplace<PlayerStPosComponent>(entity);
    pos.x = x;
    pos.y = ground_y;
    pos.z = z;
    pos.yaw = 0.0f;

    registry.emplace<PlayerStVelComponent>(entity);

    auto& physics = registry.emplace<PlayerStPhysComponent>(entity);
    physics.on_ground = true;
    physics.gravity_enabled = true;

    auto& state = registry.emplace<PlayerStStsComponent>(entity);
    state.state = PLAYER_STATE_IDLE;

    auto& chunk = registry.emplace<PlayerStChkComponent>(entity);
    chunk.chunk_x = static_cast<int32_t>(std::floor(x / mov.chunk_size));
    chunk.chunk_y = static_cast<int32_t>(std::floor(z / mov.chunk_size));

    registry.emplace<input::InputStateMoveComponent>(entity);
    registry.emplace<input::InputStateActionComponent>(entity);
    registry.emplace<input::InputStateCameraComponent>(entity);
    auto& input_meta = registry.emplace<input::InputStateMetaComponent>(entity);
    input_meta.controller_id = player_id;
    input_meta.last_update = std::chrono::steady_clock::now();

    registry.emplace<input::InputLocalTag>(entity);

    auto& cam_pos = registry.emplace<camera::CameraStPosComponent>(entity);
    cam_pos.tgt_x = x;
    cam_pos.tgt_y = ground_y + mov.eye_height;
    cam_pos.tgt_z = z;
    cam_pos.pos_x = x;
    cam_pos.pos_y = ground_y + mov.eye_height + 15.0f;
    cam_pos.pos_z = z + 15.0f;

    auto& cam_orient = registry.emplace<camera::CameraStOrtComponent>(entity);
    cam_orient.pitch = 0.4f;

    registry.emplace<camera::CameraStInpComponent>(entity);

    auto& cam_orbit = registry.emplace<camera::CameraStOrbComponent>(entity);
    cam_orbit.distance = 15.0f;
    cam_orbit.target_distance = 15.0f;

    auto& cam_target = registry.emplace<camera::CameraStTgtComponent>(entity);
    cam_target.target_entity = static_cast<uint32_t>(entity);

    registry.emplace<camera::CameraTpTag>(entity);
    registry.emplace<camera::CameraActTag>(entity);

    registry.emplace<PlayerSpawnedTag>(entity);
    registry.emplace<PlayerDirtyTag>(entity);

    // Terrain streaming: player is an observer that needs chunks loaded
    auto& strm_obs = registry.emplace<terrain::TerrainStrmObsComponent>(entity);
    strm_obs.chunk_x = chunk.chunk_x;
    strm_obs.chunk_y = chunk.chunk_y;
    strm_obs.needs_update = true;  // Trigger initial chunk loading

    return entity;
}

void process_spawn_requests(ecs::Registry& registry) {
    auto view = registry.view<PlayerReqSpawnComponent>();
    auto player_view = registry.view<PlayerStIdComponent>();  // View created once

    for (auto [request_entity, request] : view.each()) {
        ecs::Entity result_entity = ecs::NullEntity;
        bool success = false;

        // INLINED: find_player_by_id() - no helper with View!
        ecs::Entity existing_player = ecs::NullEntity;
        for (auto [pe, identity] : player_view.each()) {
            if (identity.player_id == request.player_id) {
                existing_player = pe;
                break;
            }
        }

        if (existing_player == ecs::NullEntity) {
            result_entity = create_player_entity(
                registry, request.player_id, request.x, request.z
            );
            success = true;
            log::info("[PlayerLifeSpawnSystem] Spawned player: {}", request.player_id);
        } else {
            log::warn("[PlayerLifeSpawnSystem] Player already exists: {}", request.player_id);
        }

        auto& result = registry.emplace<PlayerReqSpawnResComponent>(request_entity);
        result.spawned_entity = result_entity;
        result.success = success;

        registry.remove<PlayerReqSpawnComponent>(request_entity);
    }
}

void process_despawn_requests(ecs::Registry& registry) {
    auto view = registry.view<PlayerReqDespComponent>();
    auto player_view = registry.view<PlayerStIdComponent>();  // View created once
    std::vector<ecs::Entity> to_destroy;

    for (auto [request_entity, request] : view.each()) {
        bool success = false;

        // INLINED: find_player_by_id() - no helper with View!
        ecs::Entity player_entity = ecs::NullEntity;
        for (auto [pe, identity] : player_view.each()) {
            if (identity.player_id == request.player_id) {
                player_entity = pe;
                break;
            }
        }

        if (player_entity != ecs::NullEntity && registry.valid(player_entity)) {
            to_destroy.push_back(player_entity);
            success = true;
            log::info("[PlayerLifeSpawnSystem] Despawned player: {}", request.player_id);
        } else {
            log::warn("[PlayerLifeSpawnSystem] Player not found: {}", request.player_id);
        }

        auto& result = registry.emplace<PlayerReqDespResComponent>(request_entity);
        result.success = success;

        registry.remove<PlayerReqDespComponent>(request_entity);
    }

    for (auto entity : to_destroy) {
        registry.destroy(entity);
    }
}

void process_network_spawn_requests(ecs::Registry& registry) {
    auto view = registry.view<network::NetworkPlrReqSpawnComponent>();
    auto player_view = registry.view<PlayerStIdComponent>();  // View created once
    std::vector<ecs::Entity> to_destroy;

    for (auto [request_entity, req] : view.each()) {
        std::string player_id(req.player_id.data());

        // INLINED: find_player_by_id() - no helper with View!
        ecs::Entity existing_player = ecs::NullEntity;
        for (auto [pe, identity] : player_view.each()) {
            if (identity.player_id == player_id) {
                existing_player = pe;
                break;
            }
        }

        if (existing_player == ecs::NullEntity) {
            create_player_entity(registry, player_id, req.x, req.z);
            log::info("[PlayerLifeSpawnSystem] Spawned player from network: {} at ({}, {})",
                     player_id, req.x, req.z);
        } else {
            log::warn("[PlayerLifeSpawnSystem] Player already exists: {}", player_id);
        }

        to_destroy.push_back(request_entity);
    }

    for (auto entity : to_destroy) {
        registry.destroy(entity);
    }
}

void process_network_despawn_requests(ecs::Registry& registry) {
    auto view = registry.view<network::NetworkPlrReqDespawnComponent>();
    auto player_view = registry.view<PlayerStIdComponent>();  // View created once
    std::vector<ecs::Entity> to_destroy_requests;
    std::vector<ecs::Entity> to_destroy_players;

    for (auto [request_entity, req] : view.each()) {
        std::string player_id(req.player_id.data());

        // INLINED: find_player_by_id() - no helper with View!
        ecs::Entity player_entity = ecs::NullEntity;
        for (auto [pe, identity] : player_view.each()) {
            if (identity.player_id == player_id) {
                player_entity = pe;
                break;
            }
        }

        if (player_entity != ecs::NullEntity && registry.valid(player_entity)) {
            to_destroy_players.push_back(player_entity);
            log::info("[PlayerLifeSpawnSystem] Despawned player from network: {}", player_id);
        } else {
            log::warn("[PlayerLifeSpawnSystem] Player not found for despawn: {}", player_id);
        }

        to_destroy_requests.push_back(request_entity);
    }

    for (auto entity : to_destroy_players) {
        registry.destroy(entity);
    }
    for (auto entity : to_destroy_requests) {
        registry.destroy(entity);
    }
}

}  // anonymous namespace

void PlayerLifeSpawnSystem::on_start(ecs::Registry& registry) {
    auto view = registry.view<PlayerMgrTag>();
    if (view.empty()) {
        auto mgr = registry.create();
        registry.emplace<PlayerMgrTag>(mgr);

        auto& mov = registry.emplace<PlayerStMovComponent>(mgr);
        mov.walk_speed = MOVEMENT_DEFAULT_WALK_SPEED;
        mov.run_speed = MOVEMENT_DEFAULT_RUN_SPEED;
        mov.jump_impulse = MOVEMENT_DEFAULT_JUMP_IMPULSE;
        mov.gravity = MOVEMENT_DEFAULT_GRAVITY;
        mov.ground_friction = MOVEMENT_DEFAULT_GROUND_FRICTION;
        mov.air_control = MOVEMENT_DEFAULT_AIR_CONTROL;
        mov.ground_snap_dist = MOVEMENT_DEFAULT_GROUND_SNAP_DIST;
        mov.turn_speed = MOVEMENT_DEFAULT_TURN_SPEED;
        mov.min_speed_threshold = MOVEMENT_DEFAULT_MIN_SPEED_THRESHOLD;
        mov.velocity_epsilon = MOVEMENT_DEFAULT_VELOCITY_EPSILON;
        mov.eye_height = MOVEMENT_DEFAULT_EYE_HEIGHT;
        mov.chunk_size = MOVEMENT_DEFAULT_CHUNK_SIZE;

        log::info("[PlayerLifeSpawnSystem] Created player manager entity");
    }
}

void PlayerLifeSpawnSystem::on_stop(ecs::Registry& registry) {
    // Despawn all players on shutdown
    std::vector<ecs::Entity> to_destroy;
    auto view = registry.view<PlayerStIdComponent>();
    for (auto entity : view) {
        to_destroy.push_back(entity);
    }

    for (auto entity : to_destroy) {
        registry.destroy(entity);
    }

    if (!to_destroy.empty()) {
        log::info("[PlayerLifeSpawnSystem] Despawned {} players on shutdown", to_destroy.size());
    }
}

void PlayerLifeSpawnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    process_spawn_requests(registry);
    process_despawn_requests(registry);
    process_network_spawn_requests(registry);
    process_network_despawn_requests(registry);
}

}  // namespace ase::player
