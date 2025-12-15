#include <ase/player/player.hpp>
#include <ase/ecs/ecs.hpp>
#include <iostream>
#include <cassert>

using namespace ase::player;

void test_player_component() {
    std::cout << "Testing PlayerComponent..." << std::endl;

    PlayerComponent player("test_player_1");
    assert(player.player_id == "test_player_1");
    assert(player.position.x == 0.0f);
    assert(player.position.y == 0.0f);
    assert(player.position.z == 0.0f);
    assert(player.on_ground == true);
    assert(player.state == PlayerState::Idle);

    std::cout << "  PlayerComponent: OK" << std::endl;
}

void test_vec3() {
    std::cout << "Testing Vec3..." << std::endl;

    Vec3 a{1.0f, 2.0f, 3.0f};
    Vec3 b{4.0f, 5.0f, 6.0f};

    auto sum = a + b;
    assert(sum.x == 5.0f);
    assert(sum.y == 7.0f);
    assert(sum.z == 9.0f);

    auto scaled = a * 2.0f;
    assert(scaled.x == 2.0f);
    assert(scaled.y == 4.0f);
    assert(scaled.z == 6.0f);

    Vec3 unit{1.0f, 0.0f, 0.0f};
    assert(std::abs(unit.length() - 1.0f) < 0.001f);

    std::cout << "  Vec3: OK" << std::endl;
}

void test_movement_config() {
    std::cout << "Testing MovementConfig..." << std::endl;

    MovementConfig config;
    assert(config.walk_speed > 0.0f);
    assert(config.run_speed > config.walk_speed);
    assert(config.gravity > 0.0f);

    std::cout << "  MovementConfig: OK" << std::endl;
}

int main() {
    std::cout << "=== ASE Player Module Tests ===" << std::endl;

    test_vec3();
    test_player_component();
    test_movement_config();

    std::cout << "=== All tests passed! ===" << std::endl;
    return 0;
}
