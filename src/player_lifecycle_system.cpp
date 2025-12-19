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
#include <ase/input/components/input_component.hpp>
#include <ase/camera/components/camera_component.hpp>
#include <ase/ecs/schedule_registry.hpp>
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

    // Input (from ase-input)
    auto& input_comp = registry.emplace<input::InputComponent>(entity);
    input_comp.controller_id = player_id;

    // Camera (from ase-camera) - use config.eye_height (SSOT)
    auto& cam = registry.emplace<camera::CameraComponent>(entity);
    cam.target = {x, ground_y + config.eye_height, z};
    cam.mode = camera::CameraMode::ThirdPerson;

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
// TICK - Process spawn/despawn requests
// =============================================================================

void PlayerLifecycleSystem::tick(ecs::Registry& registry, float /*dt*/) {
    process_spawn_requests(registry);
    process_despawn_requests(registry);
}

// =============================================================================
// REGISTRATION
// =============================================================================

// Run BEFORE other player systems - lifecycle must complete first
REGISTER_SYSTEM(PlayerLifecycleSystem)
    .in_schedule(ecs::Schedule::FixedUpdate)
    .with_priority(69);  // Before PlayerInputSystem (70)

}  // namespace ase::player
