#include <ase/player/systems/lifecycle/player_life_spawn_system.hpp>
#include <ase/player/components/request/player_req_spawn_component.hpp>
#include <ase/player/components/request/player_req_desp_component.hpp>
#include <ase/player/components/request/player_req_spawn_res_component.hpp>
#include <ase/player/components/request/player_req_desp_res_component.hpp>
#include <ase/player/components/state/player_state_id_component.hpp>
#include <ase/player/components/state/player_state_pos_component.hpp>
#include <ase/player/components/state/player_state_vel_component.hpp>
#include <ase/player/components/state/player_state_phys_component.hpp>
#include <ase/player/components/state/player_state_status_component.hpp>
#include <ase/player/components/state/player_state_chunk_component.hpp>
#include <ase/player/components/state/player_state_cfg_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/components/tag/player_tag_mgr_component.hpp>
#include <ase/player/types.hpp>
#include <ase/input/input.hpp>
#include <ase/camera/camera.hpp>
#include <ase/terrain/components/tag/terrain_tag_strm_obs_component.hpp>
#include <ase/network/components/player/req/network_plr_req_spawn_component.hpp>
#include <ase/network/components/player/req/network_plr_req_despawn_component.hpp>
#include <ase/network/components/player/state/network_plr_state_input_component.hpp>
#include <ase/log/log.hpp>

#include <cmath>
#include <vector>

namespace ase::player {

namespace {

ecs::Entity find_player_by_id(ecs::Registry& registry, const std::string& player_id) {
    auto view = registry.view<PlayerStateIdComponent>();
    for (auto [entity, identity] : view.each()) {
        if (identity.player_id == player_id) {
            return entity;
        }
    }
    return ecs::NullEntity;
}

float get_ground_height(ecs::Registry& registry, float x, float z) {
    auto config_view = registry.view<PlayerStateCfgComponent>();
    for (auto [config_entity, config] : config_view.each()) {
        if (config.height_query) {
            return config.height_query(x, z);
        }
        break;
    }
    return 0.0f;
}

MovementConfig get_movement_config(ecs::Registry& registry) {
    auto config_view = registry.view<PlayerStateCfgComponent>();
    for (auto [config_entity, config] : config_view.each()) {
        return config.movement;
    }
    return MovementConfig{};
}

ecs::Entity create_player_entity(
    ecs::Registry& registry,
    const std::string& player_id,
    float x, float z
) {
    float ground_y = get_ground_height(registry, x, z);
    MovementConfig config = get_movement_config(registry);

    auto entity = registry.create();

    auto& identity = registry.emplace<PlayerStateIdComponent>(entity);
    identity.player_id = player_id;
    identity.spawned_at = std::chrono::steady_clock::now();
    identity.last_input = identity.spawned_at;

    auto& pos = registry.emplace<PlayerStatePosComponent>(entity);
    pos.x = x;
    pos.y = ground_y;
    pos.z = z;
    pos.yaw = 0.0f;

    registry.emplace<PlayerStateVelComponent>(entity);

    auto& physics = registry.emplace<PlayerStatePhysComponent>(entity);
    physics.on_ground = true;
    physics.gravity_enabled = true;

    auto& state = registry.emplace<PlayerStateStatusComponent>(entity);
    state.state = PlayerState::Idle;

    auto& chunk = registry.emplace<PlayerStateChunkComponent>(entity);
    chunk.chunk_x = static_cast<int32_t>(std::floor(x / config.chunk_size));
    chunk.chunk_y = static_cast<int32_t>(std::floor(z / config.chunk_size));

    registry.emplace<input::InputStateMoveComponent>(entity);
    registry.emplace<input::InputStateActionComponent>(entity);
    registry.emplace<input::InputStateCameraComponent>(entity);
    auto& input_meta = registry.emplace<input::InputStateMetaComponent>(entity);
    input_meta.controller_id = player_id;
    input_meta.last_update = std::chrono::steady_clock::now();

    registry.emplace<input::InputLocalTag>(entity);

    auto& cam_pos = registry.emplace<camera::CameraStatePosComponent>(entity);
    cam_pos.target = {x, ground_y + config.eye_height, z};
    cam_pos.position = {x, ground_y + config.eye_height + 15.0f, z + 15.0f};

    auto& cam_orient = registry.emplace<camera::CameraStateOrientComponent>(entity);
    cam_orient.pitch = 0.4f;

    registry.emplace<camera::CameraStateInputComponent>(entity);

    auto& cam_orbit = registry.emplace<camera::CameraStateOrbitComponent>(entity);
    cam_orbit.distance = 15.0f;
    cam_orbit.target_distance = 15.0f;

    auto& cam_target = registry.emplace<camera::CameraStateTgtComponent>(entity);
    cam_target.target_entity = static_cast<uint32_t>(entity);

    registry.emplace<camera::CameraTpTag>(entity);
    registry.emplace<camera::CameraActiveTag>(entity);

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

    for (auto [request_entity, request] : view.each()) {
        ecs::Entity result_entity = ecs::NullEntity;
        bool success = false;

        if (find_player_by_id(registry, request.player_id) == ecs::NullEntity) {
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
    std::vector<ecs::Entity> to_destroy;

    for (auto [request_entity, request] : view.each()) {
        bool success = false;

        auto player_entity = find_player_by_id(registry, request.player_id);
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
    std::vector<ecs::Entity> to_destroy;

    for (auto [request_entity, req] : view.each()) {
        std::string player_id(req.player_id.data());

        if (find_player_by_id(registry, player_id) == ecs::NullEntity) {
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
    std::vector<ecs::Entity> to_destroy_requests;
    std::vector<ecs::Entity> to_destroy_players;

    for (auto [request_entity, req] : view.each()) {
        std::string player_id(req.player_id.data());

        auto player_entity = find_player_by_id(registry, player_id);
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
    // Create manager entity with config if not exists
    auto view = registry.view<PlayerMgrTag>();
    if (view.empty()) {
        auto mgr = registry.create();
        registry.emplace<PlayerMgrTag>(mgr);
        registry.emplace<PlayerStateCfgComponent>(mgr);
        log::info("[PlayerLifeSpawnSystem] Created player manager entity");
    }
}

void PlayerLifeSpawnSystem::on_stop(ecs::Registry& registry) {
    // Despawn all players on shutdown
    std::vector<ecs::Entity> to_destroy;
    auto view = registry.view<PlayerStateIdComponent>();
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
