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
    src/lifecycle/player_life_spwn_sys.cpp
    src/control/player_ctrl_inp_sys.cpp
    src/control/player_ctrl_mov_sys.cpp
    src/simulation/player_sim_phys_sys.cpp
    src/state/player_sta_sts_sys.cpp
    src/state/player_sta_chnk_sys.cpp
    # Hub (PLAN_ASE_SDK_V2: Write positions for L4 plugins)
    src/hub/player_hub_pos_sys.cpp
    # Network (Pure ECS 4-System Pattern)
    src/network/player_bct_req_sys.cpp       # AUSGEHEND Step 1
    src/network/player_bct_snd_sys.cpp       # AUSGEHEND Step 3
    src/persistence/player_pst_ser_sys.cpp   # Persistence via Replication Layer
    src/log/player_log_obsv_sys.cpp
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
        ase::hub      # Star Topology - write positions for L4 plugins
    PRIVATE
        # Network (Only ase::replication - re-exports serial components!)
        ase::replication
        # VERBOTEN: ase::serial, nlohmann_json
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
