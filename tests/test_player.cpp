#include <ase/player/player.hpp>
#include <ase/ecs/ecs.hpp>
#include <iostream>
#include <cassert>
#include <cmath>

using namespace ase;
using namespace ase::player;

namespace {

// Process spawn request synchronously by running lifecycle system
ase::ecs::Entity do_spawn_request(
    ase::ecs::Registry& registry,
    PlayerLifeSpawnSystem& lifecycle,
    const std::string& player_id,
    float x, float z
) {
    auto request_entity = registry.create();
    auto& request = registry.emplace<PlayerReqSpawnComponent>(request_entity);
    request.player_id = player_id;
    request.x = x;
    request.z = z;

    lifecycle.tick(registry, 0.0f);

    ase::ecs::Entity spawned_entity = ase::ecs::NullEntity;
    auto* result = registry.try_get<PlayerReqSpawnResComponent>(request_entity);
    if (result && result->success) {
        spawned_entity = result->spawned_entity;
    }

    registry.destroy(request_entity);
    return spawned_entity;
}

bool do_despawn_request(
    ase::ecs::Registry& registry,
    PlayerLifeSpawnSystem& lifecycle,
    const std::string& player_id
) {
    auto request_entity = registry.create();
    auto& request = registry.emplace<PlayerReqDespComponent>(request_entity);
    request.player_id = player_id;

    lifecycle.tick(registry, 0.0f);

    bool success = false;
    auto* result = registry.try_get<PlayerReqDespResComponent>(request_entity);
    if (result) {
        success = result->success;
    }

    registry.destroy(request_entity);
    return success;
}

ase::ecs::Entity do_find_player(ase::ecs::Registry& registry, const std::string& player_id) {
    auto view = registry.view<PlayerStateIdComponent>();
    for (auto [entity, identity] : view.each()) {
        if (identity.player_id == player_id) {
            return entity;
        }
    }
    return ase::ecs::NullEntity;
}

std::vector<std::pair<std::string, ase::ecs::Entity>> do_get_all_players(ase::ecs::Registry& registry) {
    std::vector<std::pair<std::string, ase::ecs::Entity>> result;
    auto view = registry.view<PlayerStateIdComponent>();
    for (auto [entity, identity] : view.each()) {
        result.emplace_back(identity.player_id, entity);
    }
    return result;
}

}  // anonymous namespace

void test_player_components() {
    std::cout << "Testing Player ECS Components..." << std::endl;

    ase::ecs::Registry registry;
    auto entity = registry.create();

    // Add PlayerStateIdComponent
    auto& identity = registry.emplace<PlayerStateIdComponent>(entity);
    identity.player_id = "test_player_1";
    identity.spawned_at = std::chrono::steady_clock::now();
    identity.last_input = identity.spawned_at;
    assert(identity.player_id == "test_player_1");

    // Add PlayerStatePosComponent
    auto& pos = registry.emplace<PlayerStatePosComponent>(entity);
    pos.x = 10.0f;
    pos.y = 5.0f;
    pos.z = 20.0f;
    pos.yaw = 1.57f;
    assert(pos.x == 10.0f);
    assert(pos.y == 5.0f);
    assert(pos.z == 20.0f);

    // Add PlayerStateVelComponent
    auto& vel = registry.emplace<PlayerStateVelComponent>(entity);
    vel.vx = 1.0f;
    vel.vy = -9.8f;
    vel.vz = 0.5f;
    assert(vel.vx == 1.0f);
    assert(vel.vy == -9.8f);

    // Add PlayerStatePhysComponent
    auto& physics = registry.emplace<PlayerStatePhysComponent>(entity);
    physics.on_ground = false;
    physics.gravity_enabled = true;
    assert(!physics.on_ground);
    assert(physics.gravity_enabled);

    // Add PlayerStateStatusComponent
    auto& state = registry.emplace<PlayerStateStatusComponent>(entity);
    state.state = PlayerState::Running;
    assert(state.state == PlayerState::Running);

    // Add PlayerStateChunkComponent
    auto& chunk = registry.emplace<PlayerStateChunkComponent>(entity);
    chunk.chunk_x = 2;
    chunk.chunk_y = 3;
    assert(chunk.chunk_x == 2);
    assert(chunk.chunk_y == 3);

    std::cout << "  Player Components: OK" << std::endl;
}

void test_movement_config() {
    std::cout << "Testing MovementConfig..." << std::endl;

    MovementConfig config;
    assert(config.walk_speed > 0.0f);
    assert(config.run_speed > config.walk_speed);
    assert(config.gravity > 0.0f);
    assert(config.jump_impulse > 0.0f);

    std::cout << "  MovementConfig: OK" << std::endl;
}

void test_player_state_enum() {
    std::cout << "Testing PlayerState enum..." << std::endl;

    assert(static_cast<int>(PlayerState::Idle) == 0);
    assert(static_cast<int>(PlayerState::Walking) == 1);
    assert(static_cast<int>(PlayerState::Running) == 2);
    assert(static_cast<int>(PlayerState::Jumping) == 3);
    assert(static_cast<int>(PlayerState::Falling) == 4);

    std::cout << "  PlayerState: OK" << std::endl;
}

void test_spawn_via_lifecycle_system() {
    std::cout << "Testing spawn via PlayerLifeSpawnSystem..." << std::endl;

    ase::ecs::Registry registry;
    PlayerLifeSpawnSystem lifecycle;

    // Create PlayerStateCfgComponent singleton (needed for spawn)
    auto config_entity = registry.create();
    auto& config = registry.emplace<PlayerStateCfgComponent>(config_entity);
    config.height_query = [](float, float) { return 10.0f; };

    // Spawn player via lifecycle system
    auto entity = do_spawn_request(registry, lifecycle, "spawn_test", 100.0f, 200.0f);
    assert(entity != ase::ecs::NullEntity);

    // Find player via view query
    auto found = do_find_player(registry, "spawn_test");
    assert(found == entity);

    // Verify components
    auto* identity = registry.try_get<PlayerStateIdComponent>(entity);
    assert(identity != nullptr);
    assert(identity->player_id == "spawn_test");

    auto* pos = registry.try_get<PlayerStatePosComponent>(entity);
    assert(pos != nullptr);
    assert(pos->x == 100.0f);
    assert(pos->y == 10.0f);  // From height_query
    assert(pos->z == 200.0f);

    auto* physics = registry.try_get<PlayerStatePhysComponent>(entity);
    assert(physics != nullptr);
    assert(physics->on_ground == true);

    // Cannot spawn duplicate
    auto dup = do_spawn_request(registry, lifecycle, "spawn_test", 0.0f, 0.0f);
    assert(dup == ase::ecs::NullEntity);

    // Find non-existent
    auto notfound = do_find_player(registry, "nonexistent");
    assert(notfound == ase::ecs::NullEntity);

    std::cout << "  spawn via lifecycle: OK" << std::endl;
}

void test_despawn_via_lifecycle_system() {
    std::cout << "Testing despawn via PlayerLifeSpawnSystem..." << std::endl;

    ase::ecs::Registry registry;
    PlayerLifeSpawnSystem lifecycle;

    // Create PlayerStateCfgComponent singleton
    auto config_entity = registry.create();
    auto& config = registry.emplace<PlayerStateCfgComponent>(config_entity);
    config.height_query = [](float, float) { return 0.0f; };

    // Spawn and despawn
    do_spawn_request(registry, lifecycle, "despawn_test", 0.0f, 0.0f);
    assert(do_find_player(registry, "despawn_test") != ase::ecs::NullEntity);

    bool result = do_despawn_request(registry, lifecycle, "despawn_test");
    assert(result == true);
    assert(do_find_player(registry, "despawn_test") == ase::ecs::NullEntity);

    // Cannot despawn again
    result = do_despawn_request(registry, lifecycle, "despawn_test");
    assert(result == false);

    std::cout << "  despawn via lifecycle: OK" << std::endl;
}

void test_get_all_players() {
    std::cout << "Testing get_all_players via view..." << std::endl;

    ase::ecs::Registry registry;
    PlayerLifeSpawnSystem lifecycle;

    // Create PlayerStateCfgComponent singleton
    auto config_entity = registry.create();
    auto& config = registry.emplace<PlayerStateCfgComponent>(config_entity);
    config.height_query = [](float, float) { return 0.0f; };

    // Spawn multiple players
    do_spawn_request(registry, lifecycle, "player_a", 0.0f, 0.0f);
    do_spawn_request(registry, lifecycle, "player_b", 10.0f, 10.0f);
    do_spawn_request(registry, lifecycle, "player_c", 20.0f, 20.0f);

    auto all = do_get_all_players(registry);
    assert(all.size() == 3);

    std::cout << "  get_all_players: OK" << std::endl;
}

int main() {
    std::cout << "=== ASE Player Module Tests (ECS) ===" << std::endl;

    test_player_components();
    test_movement_config();
    test_player_state_enum();
    test_spawn_via_lifecycle_system();
    test_despawn_via_lifecycle_system();
    test_get_all_players();

    std::cout << "=== All tests passed! ===" << std::endl;
    return 0;
}
