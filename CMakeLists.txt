cmake_minimum_required(VERSION 3.20)

#[[
ASE MODULE BUILD CONFIGURATION

@file        CMakeLists.txt
@brief       Build configuration for ase-player
@description Player module - ECS-based player movement and persistence.
             Lifecycle, control, simulation, state, and hub systems.

@module      ase-player
@layer       3 (Module)
@created     2025-12-15
@modified    2026-01-30
@version     00.01.01.00051 [core]

CMAKE MODULE COMPLIANCE

[ ] cmake_minimum_required(VERSION 3.20) as FIRST line
[ ] project() with VERSION and LANGUAGES CXX
[ ] message(STATUS) for configuration output
[ ] add_library() with SHARED and EXPLICIT file list
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

# Generate version.hpp from template (SSOT)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ase/player/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/ase/player/version.hpp"
    @ONLY
)

# ==============================================================================
# BUILD CONFIGURATION
# ==============================================================================



message(STATUS "Configuring ${PROJECT_NAME} v${PROJECT_VERSION}")

add_library(${PROJECT_NAME} SHARED
    src/module_export.cpp
    #[[
    HEADER FILES - ROOT
    Module entry point and type definitions.
    ]]
    include/ase/player/types.hpp
    include/ase/player/player.hpp
    include/ase/player/player_module.hpp

    #[[
    HEADER FILES - COMPONENTS (cache/)
    Cached data for observer patterns.
    ]]
    include/ase/player/components/cache/player_cache_obs_component.hpp

    #[[
    HEADER FILES - COMPONENTS (request/)
    Request components for spawn/despawn operations.
    ]]
    include/ase/player/components/request/player_req_desp_component.hpp
    include/ase/player/components/request/player_req_desp_res_component.hpp
    include/ase/player/components/request/player_req_spawn_component.hpp
    include/ase/player/components/request/player_req_spawn_res_component.hpp

    #[[
    HEADER FILES - COMPONENTS (state/)
    Runtime player state data.
    ]]
    include/ase/player/components/state/player_st_chk_component.hpp
    include/ase/player/components/state/player_st_id_component.hpp
    include/ase/player/components/state/player_st_inp_cam_component.hpp
    include/ase/player/components/state/player_st_inp_mov_component.hpp
    include/ase/player/components/state/player_st_inp_net_component.hpp
    include/ase/player/components/state/player_st_mov_component.hpp
    include/ase/player/components/state/player_st_phys_component.hpp
    include/ase/player/components/state/player_st_pos_component.hpp
    include/ase/player/components/state/player_st_sts_component.hpp
    include/ase/player/components/state/player_st_vel_component.hpp
    include/ase/player/components/state/player_sta_cht_comp.hpp
    include/ase/player/components/state/player_sta_acmp_comp.hpp
    include/ase/player/components/state/player_sta_act_comp.hpp
    include/ase/player/components/state/player_sta_cmb_comp.hpp
    include/ase/player/components/state/player_sta_com_comp.hpp
    include/ase/player/components/state/player_sta_eco_comp.hpp
    include/ase/player/components/state/player_sta_inv_comp.hpp

    #[[
    HEADER FILES - COMPONENTS (input/)
    External input components.
    ]]
    include/ase/player/components/input/player_inp_ext_component.hpp

    #[[
    HEADER FILES - COMPONENTS (buffer/)
    Buffer components for persistence.
    ]]
    include/ase/player/components/buffer/player_buf_pst_component.hpp

    #[[
    HEADER FILES - COMPONENTS (tag/)
    Tag components for entity classification.
    ]]
    include/ase/player/components/tag/player_tag_chunk_changed_component.hpp
    include/ase/player/components/tag/player_tag_desp_pnd_component.hpp
    include/ase/player/components/tag/player_tag_dirty_component.hpp
    include/ase/player/components/tag/player_tag_init_component.hpp
    include/ase/player/components/tag/player_tag_inp_rmv_component.hpp
    include/ase/player/components/tag/player_tag_local_component.hpp
    include/ase/player/components/tag/player_tag_mgr_component.hpp
    include/ase/player/components/tag/player_tag_pst_dty_component.hpp
    include/ase/player/components/tag/player_tag_spawned_component.hpp

    #[[
    HEADER FILES - SYSTEMS (control/)
    Player input and movement control systems.
    ]]
    include/ase/player/systems/control/player_ctrl_inp_sys.hpp
    include/ase/player/systems/control/player_ctrl_mov_sys.hpp

    #[[
    HEADER FILES - SYSTEMS (hub/)
    Hub communication systems for cross-module data.
    ]]
    include/ase/player/systems/hub/player_hub_pos_sys.hpp

    #[[
    HEADER FILES - SYSTEMS (anticheat/)
    Engine-level anti-cheat authorities (SERVER-ONLY, Phase 13 / Task 13.10 + AP-3):
    movement plus the archetype contract-topic detectors.
    ]]
    include/ase/player/systems/anticheat/player_acc_mov_sys.hpp
    include/ase/player/systems/anticheat/player_acc_act_sys.hpp
    include/ase/player/systems/anticheat/player_acc_cmb_sys.hpp
    include/ase/player/systems/anticheat/player_acc_com_sys.hpp
    include/ase/player/systems/anticheat/player_acc_eco_sys.hpp
    include/ase/player/systems/anticheat/player_acc_inv_sys.hpp

    #[[
    HEADER FILES - SYSTEMS (lifecycle/)
    Player spawn and despawn management systems.
    ]]
    include/ase/player/systems/lifecycle/player_life_spwn_sys.hpp

    #[[
    HEADER FILES - SYSTEMS (reception/)
    Backend-driven spawn: drain the Replica-forwarded BIN_MSG_PLAYER_SPAWN=77 (LANE_SPW) into a
    PlayerReqSpawnComponent (Phase 13 Task 13.10).
    ]]
    include/ase/player/systems/reception/player_spwn_rcv_sys.hpp

    #[[
    HEADER FILES - SYSTEMS (log/)
    Logging and diagnostics systems.
    ]]
    include/ase/player/systems/log/player_log_obsv_sys.hpp

    #[[
    HEADER FILES - SYSTEMS (persistence/)
    Player persistence systems.
    ]]
    include/ase/player/systems/persistence/player_pst_ser_sys.hpp

    #[[
    HEADER FILES - SYSTEMS (simulation/)
    Player physics simulation + backend cheat-induction systems (Phase 13 AP-3 archetype levers).
    ]]
    include/ase/player/systems/simulation/player_sim_phys_sys.hpp
    include/ase/player/systems/simulation/player_sim_cht_sys.hpp
    include/ase/player/systems/simulation/player_sim_act_sys.hpp
    include/ase/player/systems/simulation/player_sim_com_sys.hpp
    include/ase/player/systems/simulation/player_sim_eco_sys.hpp

    #[[
    HEADER FILES - SYSTEMS (state/)
    Player state management systems.
    ]]
    include/ase/player/systems/state/player_sta_chnk_sys.hpp
    include/ase/player/systems/state/player_sta_sts_sys.hpp

    #[[
    HEADER FILES - SYSTEMS (sync/)
    Input synchronization systems.
    ]]
    include/ase/player/systems/sync/player_sync_inp_sys.hpp

    #[[
    SOURCE FILES - RECEPTION
    Backend-driven spawn: drain BIN_MSG_PLAYER_SPAWN=77 (LANE_SPW) → PlayerReqSpawnComponent (Task 13.10).
    ]]
    src/reception/player_spwn_rcv_sys.cpp

    #[[
    SOURCE FILES - LIFECYCLE
    Player spawn and despawn management.
    ]]
    src/lifecycle/player_life_spwn_sys.cpp

    #[[
    SOURCE FILES - CONTROL
    Player input and movement control.
    ]]
    src/control/player_ctrl_inp_sys.cpp
    src/control/player_ctrl_mov_sys.cpp

    #[[
    SOURCE FILES - SIMULATION
    Player physics simulation + backend cheat-induction (Phase 13 AP-3 archetype levers).
    ]]
    src/simulation/player_sim_phys_sys.cpp
    src/simulation/player_sim_cht_sys.cpp
    src/simulation/player_sim_act_sys.cpp
    src/simulation/player_sim_com_sys.cpp
    src/simulation/player_sim_eco_sys.cpp

    #[[
    SOURCE FILES - STATE
    Player state and chunk association.
    ]]
    src/state/player_sta_sts_sys.cpp
    src/state/player_sta_chnk_sys.cpp

    #[[
    SOURCE FILES - HUB (Star Topology)
    Cross-module communication via Hub pattern.
    Write positions for L4 plugins (PLAN_ASE_SDK_V2).
    ]]
    src/hub/player_hub_pos_sys.cpp

    #[[
    SOURCE FILES - ANTICHEAT
    Engine-level anti-cheat authorities (SERVER-ONLY, Phase 13 / Task 13.10 + AP-3):
    movement plus the archetype contract-topic detectors.
    ]]
    src/anticheat/player_acc_mov_sys.cpp
    src/anticheat/player_acc_act_sys.cpp
    src/anticheat/player_acc_cmb_sys.cpp
    src/anticheat/player_acc_com_sys.cpp
    src/anticheat/player_acc_eco_sys.cpp
    src/anticheat/player_acc_inv_sys.cpp

    #[[
    SOURCE FILES - PERSISTENCE
    Player persistence via Replication Layer.
    ]]
    src/persistence/player_pst_ser_sys.cpp

    #[[
    SOURCE FILES - MIGRATION (Compute Phase 06, WS-H.4)
    PlayerSnap serializer for the boundary migrate: bumps the per-player monotonic epoch and
    encodes the frozen 42-byte PlayerSnap (ase/types/region_wire.hpp) from the live
    player_st_* components into PlayerBufMigComponent; the World tier ships frame 100.
    ]]
    src/migration/player_mig_ser_sys.cpp

    #[[
    SOURCE FILES - SYNC
    Input synchronization from network.
    ]]
    src/sync/player_sync_inp_sys.cpp

    #[[
    SOURCE FILES - LOG
    Logging and diagnostics systems.
    Observer logging for player state.
    ]]
    src/log/player_log_obsv_sys.cpp
)
add_library(ase::player ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "ase_player"
    SUFFIX ".module"
)

ase_install_module_toml(${PROJECT_NAME})

# Header files are already tracked via add_library() above
# This ensures rebuilds when component/system registrations change

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ase::ecs          # Layer 1 - ECS framework
        ase::log::headers          # Layer 1 - Logging
        ase::math         # Layer 0 - Vec3, math utilities
        ase::types        # Layer 0 - Type definitions (sentinel checks)
        ase::input        # Input types and components
        ase::terrain      # Terrain components
    PRIVATE
        ase::hub          # Layer 3 - Star Topology (write positions for L4 plugins)
        ase::transport    # Layer 1 - ctx-shared inbound FIFO lanes (PlayerSpwnRcvSystem drains LANE_SPW; websocket-FREE, L3→L1 allowed)
)
#[[ VERBOTEN: ase::replication, ase::network, ase::serial, nlohmann_json! Hub handles I/O! ]]

if(ASE_BUILD_TESTS)
    add_subdirectory(tests)
endif()

include(GNUInstallDirs)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
