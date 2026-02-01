cmake_minimum_required(VERSION 3.20)

#[[
ASE MODULE BUILD CONFIGURATION

@file        CMakeLists.txt
@brief       Build configuration for ase-player
@description Player module - ECS-based player movement and replication.
             Lifecycle, control, simulation, state, and network systems.

@module      ase-player
@layer       3 (Module)
@created     2025-12-15
@modified    2026-01-30
@version     00.01.01.00051 [core]

CMAKE MODULE COMPLIANCE

[ ] cmake_minimum_required(VERSION 3.20) as FIRST line
[ ] project() with VERSION and LANGUAGES CXX
[ ] message(STATUS) for configuration output
[ ] add_library() with STATIC and EXPLICIT file list
[ ] NO file(GLOB_RECURSE ...) - explicit files only!
[ ] NO file(GLOB ...) - explicit files only!
[ ] Files organized by category with bracket comment headers
[ ] add_library(ase::{module} ALIAS ...) for namespace
[ ] target_include_directories() with PUBLIC and generator expressions
[ ] target_compile_features() with cxx_std_20
[ ] target_link_libraries() with PUBLIC/PRIVATE separation
[ ] Layer dependencies correct (L3 → L0, L1, L2 only)
[ ] ase::hub for cross-module communication (Star Topology)
[ ] VERBOTEN comment for forbidden dependencies
[ ] if(ASE_BUILD_TESTS) block for tests
[ ] add_subdirectory(tests) inside test block
[ ] include(GNUInstallDirs) for installation
[ ] install(DIRECTORY include/ ...) for headers
]]

# Parse local VERSION (SSOT) if not building from root
if(NOT DEFINED ASE_PLAYER_VERSION)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
        message(FATAL_ERROR "VERSION file not found: ${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
    endif()
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" _VERSION_LINES)
    foreach(_LINE ${_VERSION_LINES})
        if(_LINE MATCHES "^ASE_PLAYER=(.*)$")
            set(ASE_PLAYER_VERSION "${CMAKE_MATCH_1}")
        endif()
    endforeach()
    if(NOT DEFINED ASE_PLAYER_VERSION)
        message(FATAL_ERROR "ASE_PLAYER version not found in VERSION file")
    endif()
endif()

project(ase-player VERSION ${ASE_PLAYER_VERSION} LANGUAGES CXX)

# ==============================================================================
# BUILD CONFIGURATION
# ==============================================================================



message(STATUS "Configuring ${PROJECT_NAME} v${PROJECT_VERSION}")

add_library(${PROJECT_NAME} STATIC
    #[[
    LIFECYCLE
    Player spawn and despawn management.
    ]]
    src/lifecycle/player_life_spwn_sys.cpp

    #[[
    CONTROL
    Player input and movement control.
    ]]
    src/control/player_ctrl_inp_sys.cpp
    src/control/player_ctrl_mov_sys.cpp

    #[[
    SIMULATION
    Player physics simulation.
    ]]
    src/simulation/player_sim_phys_sys.cpp

    #[[
    STATE
    Player state and chunk association.
    ]]
    src/state/player_sta_sts_sys.cpp
    src/state/player_sta_chnk_sys.cpp

    #[[
    HUB (Star Topology)
    Cross-module communication via Hub pattern.
    Write positions for L4 plugins (PLAN_ASE_SDK_V2).
    ]]
    src/hub/player_hub_pos_sys.cpp

    #[[
    NETWORK
    Network broadcast systems (Pure ECS 4-System Pattern).
    Outgoing broadcast request and send.
    ]]
    src/network/player_bct_req_sys.cpp
    src/network/player_bct_snd_sys.cpp

    #[[
    PERSISTENCE
    Player persistence via Replication Layer.
    ]]
    src/persistence/player_pst_ser_sys.cpp

    #[[
    SYNC
    Input synchronization from network.
    ]]
    src/sync/player_sync_inp_sys.cpp

    #[[
    LOG
    Logging and diagnostics systems.
    Observer logging for player state.
    ]]
    src/log/player_log_obsv_sys.cpp
)
add_library(ase::player ALIAS ${PROJECT_NAME})

# Track module definition header - forces rebuild when module registration changes
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ase/player/player_module.hpp
    PROPERTIES HEADER_FILE_ONLY TRUE
)
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ase/player/player_module.hpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ase::ecs          # Layer 1 - ECS framework
        ase::log          # Layer 1 - Logging
        ase::math         # Layer 0 - Vec3, math utilities
        ase::input        # Input types and components
        ase::terrain      # Terrain components
        ase::camera       # CameraComponent for spawn_player
        ase::network      # ECS communication via network components
    PRIVATE
        ase::hub          # Layer 1 - Star Topology (write positions for L4 plugins)
        ase::replication  # Only ase::replication - re-exports serial components!
)
#[[ VERBOTEN: ase::serial, nlohmann_json! Use ase::replication only! ]]

if(ASE_BUILD_TESTS)
    add_subdirectory(tests)
endif()

include(GNUInstallDirs)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
