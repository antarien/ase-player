#include <ase/player/systems/player_lifecycle_system.hpp>
#include <ase/player/components/player_spawn_request_component.hpp>
#include <ase/player/components/player_despawn_request_component.hpp>
#include <ase/player/components/player_spawn_result_component.hpp>
#include <ase/player/components/player_despawn_result_component.hpp>
#include <ase/player/components/player_identity_component.hpp>
#include <ase/player/components/player_position_component.hpp>
#include <ase/player/components/player_velocity_component.hpp>
#include <ase/player/components/player_physics_component.hpp>
#include <ase/player/components/player_state_component.hpp>
#include <ase/player/components/player_chunk_component.hpp>
#include <ase/player/components/player_config_component.hpp>
#include <ase/player/components/player_tags.hpp>
#include <ase/player/types.hpp>
#include <ase/input/components/input_movement_component.hpp>
#include <ase/input/components/input_action_component.hpp>
#include <ase/input/components/input_camera_component.hpp>
#include <ase/input/components/input_meta_component.hpp>
#include <ase/input/components/input_tags.hpp>
// ase-camera (ECS-compliant - separate components + tags)
#include <ase/camera/components/camera_position_component.hpp>
#include <ase/camera/components/camera_orientation_component.hpp>
#include <ase/camera/components/camera_input_component.hpp>
#include <ase/camera/components/camera_orbit_component.hpp>
#include <ase/camera/components/camera_config_component.hpp>
#include <ase/camera/components/camera_target_component.hpp>
#include <ase/camera/components/camera_tags.hpp>
// ase-network (ECS communication via Components)
#include <ase/network/components/player/req/network_plr_req_spawn_component.hpp>
#include <ase/network/components/player/req/network_plr_req_despawn_component.hpp>
#include <ase/network/components/player/state/network_plr_state_input_component.hpp>
#include <ase/log/log.hpp>

#include <cmath>
#include <vector>

namespace ase::player {

namespace {

// Find player entity by player_id
ecs::Entity find_player_by_id(ecs::Registry& registry, const std::string& player_id) {
    auto view = registry.view<PlayerIdentityComponent>();
    for (auto [entity, identity] : view.each()) {
        if (identity.player_id == player_id) {
            return entity;
        }
    }
    return ecs::NullEntity;
}

// Get height from PlayerConfigComponent singleton
float get_ground_height(ecs::Registry& registry, float x, float z) {
    auto config_view = registry.view<PlayerConfigComponent>();
    for (auto [config_entity, config] : config_view.each()) {
        if (config.height_query) {
            return config.height_query(x, z);
        }
        break;  // Only one config entity
    }
    return 0.0f;
}

// Get MovementConfig from singleton
MovementConfig get_movement_config(ecs::Registry& registry) {
    auto config_view = registry.view<PlayerConfigComponent>();
    for (auto [config_entity, config] : config_view.each()) {
        return config.movement;
    }
    return MovementConfig{};  // Default if no config
}

// Create player entity with all required components
ecs::Entity create_player_entity(
    ecs::Registry& registry,
    const std::string& player_id,
    float x, float z
) {
    float ground_y = get_ground_height(registry, x, z);
    MovementConfig config = get_movement_config(registry);

    auto entity = registry.create();

    // Identity
    auto& identity = registry.emplace<PlayerIdentityComponent>(entity);
    identity.player_id = player_id;
    identity.spawned_at = std::chrono::steady_clock::now();
    identity.last_input = identity.spawned_at;

    // Position
    auto& pos = registry.emplace<PlayerPositionComponent>(entity);
    pos.x = x;
    pos.y = ground_y;
    pos.z = z;
    pos.yaw = 0.0f;

    // Velocity
    registry.emplace<PlayerVelocityComponent>(entity);

    // Physics
    auto& physics = registry.emplace<PlayerPhysicsComponent>(entity);
    physics.on_ground = true;
    physics.gravity_enabled = true;

    // State
    auto& state = registry.emplace<PlayerStateComponent>(entity);
    state.state = PlayerState::Idle;

    // Chunk (use config.chunk_size - SSOT)
    auto& chunk = registry.emplace<PlayerChunkComponent>(entity);
    chunk.chunk_x = static_cast<int32_t>(std::floor(x / config.chunk_size));
    chunk.chunk_y = static_cast<int32_t>(std::floor(z / config.chunk_size));

    // Input Components (from ase-input - split by concern)
    registry.emplace<input::InputMovementComponent>(entity);
    registry.emplace<input::InputActionComponent>(entity);
    registry.emplace<input::InputCameraComponent>(entity);
    auto& input_meta = registry.emplace<input::InputMetaComponent>(entity);
    input_meta.controller_id = player_id;
    input_meta.last_update = std::chrono::steady_clock::now();

    // Input Tag: LocalControlledTag for local player (set by caller if needed)
    registry.emplace<input::LocalControlledTag>(entity);

    // Camera Components (from ase-camera - split by concern)
    auto& cam_pos = registry.emplace<camera::CameraPositionComponent>(entity);
    cam_pos.target = {x, ground_y + config.eye_height, z};
    cam_pos.position = {x, ground_y + config.eye_height + 15.0f, z + 15.0f};  // Initial orbit position

    auto& cam_orient = registry.emplace<camera::CameraOrientationComponent>(entity);
    cam_orient.pitch = 0.4f;  // Default pitch

    registry.emplace<camera::CameraInputComponent>(entity);

    auto& cam_orbit = registry.emplace<camera::CameraOrbitComponent>(entity);
    cam_orbit.distance = 15.0f;
    cam_orbit.target_distance = 15.0f;

    registry.emplace<camera::CameraConfigComponent>(entity);

    auto& cam_target = registry.emplace<camera::CameraTargetComponent>(entity);
    cam_target.target_entity = static_cast<uint32_t>(entity);

    // Camera Mode: ThirdPerson (Tag statt enum!)
    registry.emplace<camera::ThirdPersonCameraTag>(entity);
    registry.emplace<camera::ActiveCameraTag>(entity);

    // Tags for broadcast
    registry.emplace<PlayerJustSpawnedTag>(entity);
    registry.emplace<PlayerDirtyTag>(entity);

    return entity;
}

// Process all spawn requests
void process_spawn_requests(ecs::Registry& registry) {
    auto view = registry.view<PlayerSpawnRequestComponent>();

    for (auto [request_entity, request] : view.each()) {
        ecs::Entity result_entity = ecs::NullEntity;
        bool success = false;

        // Check if player already exists
        if (find_player_by_id(registry, request.player_id) == ecs::NullEntity) {
            result_entity = create_player_entity(
                registry, request.player_id, request.x, request.z
            );
            success = true;
            log::info("[PlayerLifecycleSystem] Spawned player: {}", request.player_id);
        } else {
            log::warn("[PlayerLifecycleSystem] Player already exists: {}", request.player_id);
        }

        // Add result component
        auto& result = registry.emplace<PlayerSpawnResultComponent>(request_entity);
        result.spawned_entity = result_entity;
        result.success = success;

        // Remove request component (processed)
        registry.remove<PlayerSpawnRequestComponent>(request_entity);
    }
}

// Process all despawn requests
void process_despawn_requests(ecs::Registry& registry) {
    auto view = registry.view<PlayerDespawnRequestComponent>();
    std::vector<ecs::Entity> to_destroy;

    for (auto [request_entity, request] : view.each()) {
        bool success = false;

        // Find player entity
        auto player_entity = find_player_by_id(registry, request.player_id);
        if (player_entity != ecs::NullEntity && registry.valid(player_entity)) {
            to_destroy.push_back(player_entity);
            success = true;
            log::info("[PlayerLifecycleSystem] Despawned player: {}", request.player_id);
        } else {
            log::warn("[PlayerLifecycleSystem] Player not found: {}", request.player_id);
        }

        // Add result component
        auto& result = registry.emplace<PlayerDespawnResultComponent>(request_entity);
        result.success = success;

        // Remove request component (processed)
        registry.remove<PlayerDespawnRequestComponent>(request_entity);
    }

    // Safe deletion: after iteration
    for (auto entity : to_destroy) {
        registry.destroy(entity);
    }
}

// ============================================================================
// Network Component Processing (ECS communication via Components)
// ============================================================================

// Process spawn requests from Network module (NetworkPlrReqSpawnComponent)
void process_network_spawn_requests(ecs::Registry& registry) {
    auto view = registry.view<network::NetworkPlrReqSpawnComponent>();
    std::vector<ecs::Entity> to_destroy;

    for (auto [request_entity, req] : view.each()) {
        std::string player_id(req.player_id.data());

        // Check if player already exists
        if (find_player_by_id(registry, player_id) == ecs::NullEntity) {
            create_player_entity(registry, player_id, req.x, req.z);
            log::info("[PlayerLifecycleSystem] Spawned player from network: {} at ({}, {})",
                     player_id, req.x, req.z);
        } else {
            log::warn("[PlayerLifecycleSystem] Player already exists: {}", player_id);
        }

        // Mark for destruction (processed)
        to_destroy.push_back(request_entity);
    }

    // Safe deletion: after iteration
    for (auto entity : to_destroy) {
        registry.destroy(entity);
    }
}

// Process despawn requests from Network module (NetworkPlrReqDespawnComponent)
void process_network_despawn_requests(ecs::Registry& registry) {
    auto view = registry.view<network::NetworkPlrReqDespawnComponent>();
    std::vector<ecs::Entity> to_destroy_requests;
    std::vector<ecs::Entity> to_destroy_players;

    for (auto [request_entity, req] : view.each()) {
        std::string player_id(req.player_id.data());

        // Find player entity
        auto player_entity = find_player_by_id(registry, player_id);
        if (player_entity != ecs::NullEntity && registry.valid(player_entity)) {
            to_destroy_players.push_back(player_entity);
            log::info("[PlayerLifecycleSystem] Despawned player from network: {}", player_id);
        } else {
            log::warn("[PlayerLifecycleSystem] Player not found for despawn: {}", player_id);
        }

        // Mark request for destruction (processed)
        to_destroy_requests.push_back(request_entity);
    }

    // Safe deletion: after iteration
    for (auto entity : to_destroy_players) {
        registry.destroy(entity);
    }
    for (auto entity : to_destroy_requests) {
        registry.destroy(entity);
    }
}

}  // anonymous namespace

// =============================================================================
// LIFECYCLE
// =============================================================================

void PlayerLifecycleSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[PlayerLifecycleSystem] Started");
}

void PlayerLifecycleSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[PlayerLifecycleSystem] Stopped");
}

// =============================================================================
// TICK - Process spawn/despawn requests (from Player AND Network modules)
// =============================================================================

void PlayerLifecycleSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // Process requests from Player module (REST API)
    process_spawn_requests(registry);
    process_despawn_requests(registry);

    // Process requests from Network module (WebRTC DataChannel)
    // ECS communication: Network Systems write Components, Player Systems read them
    process_network_spawn_requests(registry);
    process_network_despawn_requests(registry);
}

// =============================================================================
// REGISTRATION
// =============================================================================

// Run BEFORE other player systems - lifecycle must complete first

}  // namespace ase::player
