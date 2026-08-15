#include <ase/player/player.hpp>
#include <ase/terrain/terrain.hpp>
#include <ase/terrain/terrain_chunk_layer_resource_manager.hpp>
#include <ase/ecs/system.hpp>
#include <ase/containers/vector.hpp>
#include <ase/containers/hash_map.hpp>
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstring>

using namespace ase;
using namespace ase::player;

namespace {

// Process spawn request synchronously by running lifecycle system
ase::ecs::Entity do_spawn_request(
    ase::ecs::Registry& registry,
    PlayerLifeSpwnSystem& lifecycle,
    const char* player_id,
    float x, float z
) {
    auto request_entity = registry.create();
    auto& request = registry.emplace<PlayerReqSpwnComponent>(request_entity);
    std::strncpy(request.player_id, player_id, sizeof(request.player_id) - 1);
    request.player_id[sizeof(request.player_id) - 1] = '\0';
    request.x = x;
    request.z = z;

    lifecycle.tick(registry, 0.0f);

    ase::ecs::Entity spawned_entity = ase::ecs::NullEntity;
    auto* result = registry.try_get<PlayerReqSpwnResComponent>(request_entity);
    if (result && result->success) {
        spawned_entity = result->spawned_entity;
    }

    registry.destroy(request_entity);
    return spawned_entity;
}

bool do_despawn_request(
    ase::ecs::Registry& registry,
    PlayerLifeSpwnSystem& lifecycle,
    const char* player_id
) {
    auto request_entity = registry.create();
    auto& request = registry.emplace<PlayerReqDespComponent>(request_entity);
    std::strncpy(request.player_id, player_id, sizeof(request.player_id) - 1);
    request.player_id[sizeof(request.player_id) - 1] = '\0';

    lifecycle.tick(registry, 0.0f);

    bool success = false;
    auto* result = registry.try_get<PlayerReqDespResComponent>(request_entity);
    if (result) {
        success = result->success;
    }

    registry.destroy(request_entity);
    return success;
}

ase::ecs::Entity do_find_player(ase::ecs::Registry& registry, const char* player_id) {
    auto view = registry.view<PlayerStaIdntComponent>();
    for (auto [entity, identity] : view.each()) {
        if (std::strcmp(identity.player_id, player_id) == 0) {
            return entity;
        }
    }
    return ase::ecs::NullEntity;
}

ase::containers::Vector<ase::containers::Pair<std::string, ase::ecs::Entity>> do_get_all_players(ase::ecs::Registry& registry) {
    ase::containers::Vector<ase::containers::Pair<std::string, ase::ecs::Entity>> result;
    auto view = registry.view<PlayerStaIdntComponent>();
    for (auto [entity, identity] : view.each()) {
        result.emplace_back(identity.player_id, entity);
    }
    return result;
}

void setup_terrain_chunk(ase::ecs::Registry& registry, int32_t chunk_x, int32_t chunk_y, float height) {
    // The layer arrays live in the ResourceManager, reachable through registry.ctx(); the chunk
    // entity id is the slot, exactly as TerrainChkSystem assigns it in production.
    static terrain::TerrainChunkLayerResourceManager layer_mgr;
    if (registry.ctx().find<terrain::TerrainChunkLayerResourceManager>() == nullptr) {
        registry.ctx().emplace<terrain::TerrainChunkLayerResourceManager&>(layer_mgr);
    }

    auto chunk = registry.create();
    auto& crd = registry.emplace<terrain::TerrainStaChkCrdComponent>(chunk);
    crd.x = chunk_x;
    crd.y = chunk_y;

    auto& lyr = registry.emplace<terrain::TerrainStaChkLyrComponent>(chunk);
    lyr.lyr_slot = static_cast<uint32_t>(chunk);

    auto& mgr = registry.ctx().get<terrain::TerrainChunkLayerResourceManager&>();
    mgr.store_layers(lyr.lyr_slot, terrain::MACRO_RESOLUTION * terrain::MACRO_RESOLUTION);

    float* hgt = mgr.get_macro_heights(lyr.lyr_slot);
    assert(hgt != nullptr);
    for (size_t i = 0; i < terrain::MACRO_RESOLUTION * terrain::MACRO_RESOLUTION; ++i) {
        hgt[i] = height;
    }
}

}  // anonymous namespace

void test_player_components() {
    std::cout << "Testing Player ECS Components..." << std::endl;

    ase::ecs::Registry registry;
    auto entity = registry.create();

    // Add PlayerStaIdntComponent
    auto& identity = registry.emplace<PlayerStaIdntComponent>(entity);
    std::strncpy(identity.player_id, "test_player_1", sizeof(identity.player_id) - 1);
    identity.player_id[sizeof(identity.player_id) - 1] = '\0';
    identity.spawned_at_ms = 1000;
    identity.last_input_ms = identity.spawned_at_ms;
    assert(std::strcmp(identity.player_id, "test_player_1") == 0);

    // Add PlayerStaPosComponent (chunk-relativ seit S2b 2026-08-11: 10 m = Wabe 0, lokal 10)
    auto& pos = registry.emplace<PlayerStaPosComponent>(entity);
    pos.chunk_x = 0;
    pos.local_x = 10.0f;
    pos.y = 5.0f;
    pos.chunk_z = 0;
    pos.local_z = 20.0f;
    assert(pos.chunk_x == 0);
    assert(pos.local_x == 10.0f);
    assert(pos.y == 5.0f);
    assert(pos.local_z == 20.0f);

    // Der Blick wohnt seit S2b in der eigenen Komponente
    auto& yaw = registry.emplace<PlayerStaYawComponent>(entity);
    yaw.yaw = 1.57f;
    assert(yaw.yaw == 1.57f);

    // Add PlayerStaVelComponent
    auto& vel = registry.emplace<PlayerStaVelComponent>(entity);
    vel.vx = 1.0f;
    vel.vy = -9.8f;
    vel.vz = 0.5f;
    assert(vel.vx == 1.0f);
    assert(vel.vy == -9.8f);

    // Add PlayerStaPhysComponent
    auto& physics = registry.emplace<PlayerStaPhysComponent>(entity);
    physics.on_ground = false;
    physics.gravity_enabled = true;
    assert(!physics.on_ground);
    assert(physics.gravity_enabled);

    // Add PlayerStaStsComponent
    auto& state = registry.emplace<PlayerStaStsComponent>(entity);
    state.sts = PLAYER_STATE_RUNNING;
    assert(state.sts == PLAYER_STATE_RUNNING);

    // Add PlayerStaChkComponent
    auto& chunk = registry.emplace<PlayerStaChkComponent>(entity);
    chunk.chunk_x = 2;
    chunk.chunk_y = 3;
    assert(chunk.chunk_x == 2);
    assert(chunk.chunk_y == 3);

    std::cout << "  Player Components: OK" << std::endl;
}

void test_movement_defaults() {
    std::cout << "Testing Movement Defaults..." << std::endl;

    assert(MOVEMENT_DEFAULT_WALK_SPEED > 0.0f);
    assert(MOVEMENT_DEFAULT_RUN_SPEED > MOVEMENT_DEFAULT_WALK_SPEED);
    assert(MOVEMENT_DEFAULT_GRAVITY > 0.0f);
    assert(MOVEMENT_DEFAULT_JUMP_IMPULSE > 0.0f);

    std::cout << "  Movement Defaults: OK" << std::endl;
}

void test_player_state_constants() {
    std::cout << "Testing PlayerState constants..." << std::endl;

    assert(PLAYER_STATE_IDLE == 0);
    assert(PLAYER_STATE_WALKING == 1);
    assert(PLAYER_STATE_RUNNING == 2);
    assert(PLAYER_STATE_JUMPING == 3);
    assert(PLAYER_STATE_FALLING == 4);

    std::cout << "  PlayerState constants: OK" << std::endl;
}

void test_spawn_via_lifecycle_system() {
    std::cout << "Testing spawn via PlayerLifeSpwnSystem..." << std::endl;

    ase::ecs::Registry registry;
    PlayerLifeSpwnSystem lifecycle;

    // Setup terrain chunk at (0,0) with height 10.0
    setup_terrain_chunk(registry, 0, 0, 10.0f);
    // Setup terrain chunk for position (100, 200) → chunk (6, 12)
    setup_terrain_chunk(registry, 6, 12, 10.0f);

    // No movement-settings singleton is needed any more: PlayerStMovComponent was a
    // per-entity copy of the MOVEMENT_DEFAULT_* constants and was removed 2026-08-15.
    // The systems read types.hpp directly.

    // Spawn player via lifecycle system
    auto entity = do_spawn_request(registry, lifecycle, "spawn_test", 100.0f, 200.0f);
    assert(entity != ase::ecs::NullEntity);

    // Find player via view query
    auto found = do_find_player(registry, "spawn_test");
    assert(found == entity);

    // Verify components
    auto* identity = registry.try_get<PlayerStaIdntComponent>(entity);
    assert(identity != nullptr);
    assert(std::strcmp(identity->player_id, "spawn_test") == 0);

    auto* pos = registry.try_get<PlayerStaPosComponent>(entity);
    assert(pos != nullptr);
    assert(pos->x == 100.0f);
    assert(pos->y == 10.0f);  // From terrain height
    assert(pos->z == 200.0f);

    auto* physics = registry.try_get<PlayerStaPhysComponent>(entity);
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
    std::cout << "Testing despawn via PlayerLifeSpwnSystem..." << std::endl;

    ase::ecs::Registry registry;
    PlayerLifeSpwnSystem lifecycle;

    // Setup terrain chunk at (0,0) with height 0.0
    setup_terrain_chunk(registry, 0, 0, 0.0f);

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
    PlayerLifeSpwnSystem lifecycle;

    // Setup terrain chunk at (0,0)
    setup_terrain_chunk(registry, 0, 0, 0.0f);
    // Setup terrain chunks for other positions
    setup_terrain_chunk(registry, 0, 0, 0.0f);
    setup_terrain_chunk(registry, 1, 1, 0.0f);

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
    test_movement_defaults();
    test_player_state_constants();
    test_spawn_via_lifecycle_system();
    test_despawn_via_lifecycle_system();
    test_get_all_players();

    std::cout << "=== All tests passed! ===" << std::endl;
    return 0;
}
