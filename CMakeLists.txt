# ==============================================================================
# ASE Player Module (ECS-Based)
# ==============================================================================

cmake_minimum_required(VERSION 3.20)

project(ase-player
    VERSION 0.1.0
    DESCRIPTION "ASE Player - ECS-based player movement and replication"
    LANGUAGES CXX
)

# ------------------------------------------------------------------------------
# Library (Static)
# ------------------------------------------------------------------------------

add_library(ase-player STATIC
    src/player_lifecycle_system.cpp
    src/player_input_system.cpp
    src/player_movement_system.cpp
    src/player_physics_system.cpp
    src/player_state_system.cpp
    src/player_chunk_system.cpp
    src/player_broadcast_system.cpp
)
add_library(ase::player ALIAS ase-player)

target_include_directories(ase-player
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ase-player PUBLIC cxx_std_20)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

target_link_libraries(ase-player
    PUBLIC
        ase::ecs
        ase::log
        ase::terrain
        ase::input    # Input types and components
        ase::math     # Vec3, math utilities
        ase::camera   # CameraComponent for spawn_player
        ase::network  # ECS communication via network components
    PRIVATE
        ase::replication  # ECS message pattern for broadcasting
        nlohmann_json::nlohmann_json
)

# ------------------------------------------------------------------------------
# Install
# ------------------------------------------------------------------------------

install(TARGETS ase-player
    EXPORT ase-player-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ase
    DESTINATION include
)

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------

if(ASE_BUILD_TESTS)
    add_executable(ase-player-test tests/test_player.cpp)
    target_link_libraries(ase-player-test PRIVATE ase::player)

    set_target_properties(ase-player-test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    add_test(NAME ase-player-test COMMAND ase-player-test)
endif()
