#include <ase/player/systems/player_system.hpp>
#include <ase/ecs/system_registry.hpp>
#include <ase/camera/components/camera_component.hpp>
#include <ase/log/log.hpp>

#include <cmath>

namespace ase::player {

void PlayerSystem::tick(ecs::Registry& registry, float dt) {
    // Process all players with InputComponent
    auto view = registry.view<PlayerComponent, input::InputComponent>();

    for (auto [entity, player, input] : view.each()) {
        // Get movement direction from camera (CameraSystem updates movement_yaw in Input phase)
        // This ensures orbit mode (CTRL) correctly decouples camera view from movement
        auto* cam = registry.try_get<camera::CameraComponent>(entity);
        if (cam) {
            player.yaw = cam->movement_yaw;
        }

        // Process movement from input
        process_movement(player, input, dt);

        // Clear input flags after processing
        input::InputSystem::clear_flags(input);

        // Update chunk presence
        auto* presence = registry.try_get<PlayerChunkPresence>(entity);
        if (presence) {
            int32_t new_chunk_x = static_cast<int32_t>(std::floor(player.position.x / 32.0f));
            int32_t new_chunk_z = static_cast<int32_t>(std::floor(player.position.z / 32.0f));

            if (new_chunk_x != presence->chunk_x || new_chunk_z != presence->chunk_y) {
                terrain::ChunkCoord old_chunk{presence->chunk_x, presence->chunk_y};
                terrain::ChunkCoord new_chunk{new_chunk_x, new_chunk_z};

                presence->chunk_x = new_chunk_x;
                presence->chunk_y = new_chunk_z;
                presence->dirty = true;

                // Notify chunk change
                if (on_chunk_change_) {
                    on_chunk_change_(player.player_id, old_chunk, new_chunk);
                }
            }
        }

        // Mark for broadcast if moved
        if (player.velocity.length() > 0.01f || !registry.all_of<PlayerDirty>(entity)) {
            registry.emplace_or_replace<PlayerDirty>(entity);
        }
    }

    // Broadcast dirty players
    auto dirty_view = registry.view<PlayerComponent, PlayerDirty>();
    for (auto [entity, player] : dirty_view.each()) {
        if (on_update_) {
            on_update_(player.player_id, player.position, player.yaw, player.state);
        }
        registry.remove<PlayerDirty>(entity);
    }

    // Handle just-spawned players
    auto spawned_view = registry.view<PlayerComponent, PlayerJustSpawned>();
    for (auto [entity, player] : spawned_view.each()) {
        if (on_spawn_) {
            on_spawn_(player.player_id, player.position);
        }
        registry.remove<PlayerJustSpawned>(entity);
    }
}

ecs::Entity PlayerSystem::spawn_player(
    ecs::Registry& registry,
    const std::string& player_id,
    const Vec3& position
) {
    // Check if already exists
    {
        std::lock_guard lock(player_map_mutex_);
        auto it = player_map_.find(player_id);
        if (it != player_map_.end() && registry.valid(it->second)) {
            log::warn("Player {} already exists", player_id);
            return it->second;
        }
    }

    // Create entity
    auto entity = registry.create();

    // Add components
    auto& player = registry.emplace<PlayerComponent>(entity, player_id);
    player.position = position;

    // Get terrain height at spawn position - spawn at ground level
    // Camera system adds +1.0f offset to look at chest, not feet
    float ground = get_terrain_height(position.x, position.z);
    player.position.y = ground;

    // Add InputComponent from ase-input module
    auto& input_comp = registry.emplace<input::InputComponent>(entity);
    input_comp.controller_id = player_id;

    auto& presence = registry.emplace<PlayerChunkPresence>(entity);
    presence.chunk_x = static_cast<int32_t>(std::floor(position.x / 32.0f));
    presence.chunk_y = static_cast<int32_t>(std::floor(position.z / 32.0f));

    registry.emplace<PlayerJustSpawned>(entity);

    // Track in map
    {
        std::lock_guard lock(player_map_mutex_);
        player_map_[player_id] = entity;
    }

    log::info("Player {} spawned at ({:.1f}, {:.1f}, {:.1f})",
        player_id, player.position.x, player.position.y, player.position.z);

    return entity;
}

bool PlayerSystem::despawn_player(ecs::Registry& registry, const std::string& player_id) {
    ecs::Entity entity;

    {
        std::lock_guard lock(player_map_mutex_);
        auto it = player_map_.find(player_id);
        if (it == player_map_.end()) {
            return false;
        }
        entity = it->second;
        player_map_.erase(it);
    }

    if (!registry.valid(entity)) {
        return false;
    }

    // Notify before destroying
    if (on_despawn_) {
        on_despawn_(player_id);
    }

    registry.destroy(entity);
    log::info("Player {} despawned", player_id);

    return true;
}

ecs::Entity PlayerSystem::find_player(const std::string& player_id) const {
    std::lock_guard lock(player_map_mutex_);
    auto it = player_map_.find(player_id);
    if (it != player_map_.end()) {
        return it->second;
    }
    return ecs::NullEntity;
}

std::vector<std::pair<std::string, ecs::Entity>> PlayerSystem::get_all_players() const {
    std::lock_guard lock(player_map_mutex_);
    std::vector<std::pair<std::string, ecs::Entity>> result;
    result.reserve(player_map_.size());
    for (const auto& [id, entity] : player_map_) {
        result.emplace_back(id, entity);
    }
    return result;
}

void PlayerSystem::apply_input(
    ecs::Registry& registry,
    const std::string& player_id,
    const MovementInput& input
) {
    auto entity = find_player(player_id);
    if (entity == ecs::NullEntity || !registry.valid(entity)) {
        return;
    }

    // Update InputComponent (from ase-input module)
    auto* input_comp = registry.try_get<input::InputComponent>(entity);
    if (input_comp) {
        input::InputSystem::set_movement(*input_comp, input);
    }

    // Note: player->yaw is now read from CameraComponent.movement_yaw during tick()
    // This ensures orbit mode (CTRL) correctly decouples camera view from movement
    auto* player = registry.try_get<PlayerComponent>(entity);
    if (player) {
        player->last_input = std::chrono::steady_clock::now();
    }
}

void PlayerSystem::process_movement(PlayerComponent& player, const input::InputComponent& input, float dt) {
    const auto& mov = input.movement;

    // Calculate movement direction based on yaw
    // Note: In Three.js, -Z is forward, so we negate forward component
    float sin_yaw = std::sin(player.yaw);
    float cos_yaw = std::cos(player.yaw);

    // Forward/backward and strafe movement (negated forward for Three.js coords)
    Vec3 move_dir{0, 0, 0};
    move_dir.x = -mov.forward * sin_yaw + mov.strafe * cos_yaw;
    move_dir.z = -mov.forward * cos_yaw - mov.strafe * sin_yaw;

    // Normalize if moving diagonally
    if (move_dir.length_xz() > 1.0f) {
        move_dir = move_dir.normalized_xz();
    }

    // Determine speed
    float speed = mov.sprint ? config_.run_speed : config_.walk_speed;
    float control = player.on_ground ? 1.0f : config_.air_control;

    // Apply movement force
    Vec3 target_velocity = move_dir * speed;

    // Smooth acceleration
    if (player.on_ground) {
        float accel = config_.ground_friction * dt;
        player.velocity.x += (target_velocity.x - player.velocity.x) * std::min(accel, 1.0f);
        player.velocity.z += (target_velocity.z - player.velocity.z) * std::min(accel, 1.0f);
    } else {
        // Air control is limited
        player.velocity.x += (target_velocity.x - player.velocity.x) * control * dt;
        player.velocity.z += (target_velocity.z - player.velocity.z) * control * dt;
    }

    // Jump
    if (mov.jump && player.on_ground) {
        player.velocity.y = config_.jump_velocity;
        player.on_ground = false;
        player.state = PlayerState::Jumping;
    }

    // Apply gravity
    if (!player.on_ground) {
        player.velocity.y -= config_.gravity * dt;
    }

    // Update position
    player.position += player.velocity * dt;

    // Ground collision
    float ground_height = get_terrain_height(player.position.x, player.position.z);

    // Small tolerance for ground detection (prevents jitter on flat surfaces)
    constexpr float GROUND_SNAP_DIST = 0.1f;

    if (player.position.y <= ground_height + GROUND_SNAP_DIST) {
        // On or slightly above ground - snap to ground
        player.position.y = ground_height;
        player.velocity.y = 0;
        player.on_ground = true;

        // Update state
        if (player.velocity.length_xz() > 0.1f) {
            player.state = mov.sprint ? PlayerState::Running : PlayerState::Walking;
        } else {
            player.state = PlayerState::Idle;
        }
    } else {
        // Above ground - should be falling
        player.on_ground = false;
        if (player.velocity.y < 0) {
            player.state = PlayerState::Falling;
        }
    }
}

float PlayerSystem::get_terrain_height(float x, float z) {
    if (height_query_) {
        return height_query_(x, z);
    }
    return 0.0f;  // Default flat ground
}

// Auto-register PlayerSystem in Agents phase (gameplay logic, after terrain)
AUTO_REGISTER_SYSTEM(
    PlayerSystem,
    ase::ecs::SystemPhase::Agents,
    (std::vector<std::string>{"ChunkLookupSystem"})
)

}  // namespace ase::player
