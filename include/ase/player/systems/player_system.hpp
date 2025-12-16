#pragma once

/**
 * PlayerSystem - ECS System for player movement
 *
 * Responsibilities:
 * - Process player inputs from InputComponent (ase-input module)
 * - Apply gravity and friction
 * - Query terrain for ground height
 * - Update player positions
 * - Mark dirty players for SSE broadcast
 */

#include <ase/ecs/ecs.hpp>
#include <ase/player/types.hpp>
#include <ase/player/components/player_component.hpp>
#include <ase/input/input.hpp>
#include <ase/terrain/types.hpp>

#include <functional>
#include <unordered_map>
#include <mutex>

namespace ase::player {

class PlayerSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerSystem"; }
    int priority() const override { return 100; }  // After terrain systems

    void tick(ecs::Registry& registry, float dt) override;

    // =========================================================================
    // Player Management
    // =========================================================================

    /**
     * Spawn a new player at world position
     */
    ecs::Entity spawn_player(
        ecs::Registry& registry,
        const std::string& player_id,
        const Vec3& position
    );

    /**
     * Despawn a player
     */
    bool despawn_player(ecs::Registry& registry, const std::string& player_id);

    /**
     * Get player entity by ID
     */
    ecs::Entity find_player(const std::string& player_id) const;

    /**
     * Apply movement input to player (updates InputComponent)
     */
    void apply_input(
        ecs::Registry& registry,
        const std::string& player_id,
        const MovementInput& input
    );

    // =========================================================================
    // Terrain Integration
    // =========================================================================

    /**
     * Callback to query terrain height at world position
     * Returns height at (world_x, world_z)
     */
    using HeightQueryCallback = std::function<float(float world_x, float world_z)>;
    void set_height_query(HeightQueryCallback callback) {
        height_query_ = std::move(callback);
    }

    // =========================================================================
    // Callbacks for SSE/Neo4j
    // =========================================================================

    /**
     * Called when a player spawns
     */
    using OnPlayerSpawn = std::function<void(const std::string& player_id, const Vec3& position)>;
    void set_on_player_spawn(OnPlayerSpawn callback) { on_spawn_ = std::move(callback); }

    /**
     * Called when a player despawns
     */
    using OnPlayerDespawn = std::function<void(const std::string& player_id)>;
    void set_on_player_despawn(OnPlayerDespawn callback) { on_despawn_ = std::move(callback); }

    /**
     * Called when player position updates (for SSE broadcast)
     */
    using OnPlayerUpdate = std::function<void(const std::string& player_id, const Vec3& position, float yaw, PlayerState state)>;
    void set_on_player_update(OnPlayerUpdate callback) { on_update_ = std::move(callback); }

    /**
     * Called when player changes chunk (for Neo4j LOCATED_IN relationship)
     */
    using OnPlayerChunkChange = std::function<void(const std::string& player_id, terrain::ChunkCoord old_chunk, terrain::ChunkCoord new_chunk)>;
    void set_on_player_chunk_change(OnPlayerChunkChange callback) { on_chunk_change_ = std::move(callback); }

    // =========================================================================
    // Configuration
    // =========================================================================

    void set_movement_config(const MovementConfig& config) { config_ = config; }
    const MovementConfig& movement_config() const { return config_; }

private:
    // Player ID -> Entity mapping
    std::unordered_map<std::string, ecs::Entity> player_map_;
    mutable std::mutex player_map_mutex_;

    // Movement configuration
    MovementConfig config_;

    // Terrain height query
    HeightQueryCallback height_query_;

    // Callbacks
    OnPlayerSpawn on_spawn_;
    OnPlayerDespawn on_despawn_;
    OnPlayerUpdate on_update_;
    OnPlayerChunkChange on_chunk_change_;

    // Internal methods
    void process_movement(PlayerComponent& player, const input::InputComponent& input, float dt);
    float get_terrain_height(float x, float z);
};

}  // namespace ase::player
