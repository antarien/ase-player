cmake_minimum_required(VERSION 3.20)

#[[
ASE MODULE BUILD CONFIGURATION

@file        CMakeLists.txt
@brief       Build configuration for ase-player
@description Player module - ECS-based player movement and persistence.
             Lifecycle, control, simulation, state, and hub systems.

@module      ase-player
@layer       3 (Module)
@usecase     usecase:100051 - resolve module dependencies
@actor       Build Edge Resolver (BLDEDGE)
@created     2025-12-15
@modified    2026-10-21
@version     00.01.28.00079 [feat]

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

    #[[
    HEADER FILES - COMPONENTS (request/)
    Request components for spawn/despawn operations.
    ]]
    include/ase/player/components/request/player_req_desp_comp.hpp
    include/ase/player/components/request/player_req_desp_res_comp.hpp
    include/ase/player/components/request/player_req_spwn_comp.hpp
    include/ase/player/components/request/player_req_roam_comp.hpp
    include/ase/player/components/request/player_req_spwn_res_comp.hpp

    #[[
    HEADER FILES - COMPONENTS (state/)
    Runtime player state data.
    ]]
    include/ase/player/components/state/player_sta_chk_comp.hpp
    include/ase/player/components/state/player_sta_idnt_comp.hpp
    include/ase/player/components/state/player_sta_inp_mov_comp.hpp
    include/ase/player/components/state/player_sta_phys_comp.hpp
    include/ase/player/components/state/player_sta_pos_comp.hpp
    include/ase/player/components/state/player_sta_sts_comp.hpp
    include/ase/player/components/state/player_sta_vel_comp.hpp
    include/ase/player/components/state/player_sta_cht_comp.hpp
    include/ase/player/components/state/player_sta_roam_comp.hpp
    include/ase/player/components/state/player_sta_acmp_comp.hpp
    include/ase/player/components/state/player_sta_act_comp.hpp
    include/ase/player/components/state/player_sta_cmb_comp.hpp
    include/ase/player/components/state/player_sta_com_comp.hpp
    include/ase/player/components/state/player_sta_eco_comp.hpp
    include/ase/player/components/state/player_sta_inv_comp.hpp
    include/ase/player/components/state/player_sta_yaw_comp.hpp
    include/ase/player/components/state/player_sta_epch_comp.hpp
    include/ase/player/components/state/player_sta_wrld_comp.hpp

    #[[
    HEADER FILES - COMPONENTS (input/)
    External input components.
    ]]
    include/ase/player/components/input/player_inp_ext_comp.hpp
    include/ase/player/components/input/player_inp_cam_comp.hpp
    include/ase/player/components/input/player_inp_trn_comp.hpp
    include/ase/player/components/input/player_inp_tlpt_comp.hpp
    include/ase/player/components/input/player_inp_mig_dlvr_comp.hpp
    include/ase/player/components/input/player_inp_mig_ack_comp.hpp

    #[[
    HEADER FILES - COMPONENTS (buffer/)
    Buffer components for persistence.
    ]]
    include/ase/player/components/buffer/player_buf_pst_comp.hpp

    #[[
    HEADER FILES - COMPONENTS (tag/)
    Tag components for entity classification.
    ]]
    include/ase/player/components/tag/player_chnk_chgd_tag.hpp
    include/ase/player/components/tag/player_desp_pnd_tag.hpp
    include/ase/player/components/tag/player_drty_tag.hpp
    include/ase/player/components/tag/player_sess_strt_tag.hpp
    include/ase/player/components/tag/player_sess_done_tag.hpp
    include/ase/player/components/tag/player_init_tag.hpp
    include/ase/player/components/tag/player_inp_rmv_tag.hpp
    include/ase/player/components/tag/player_locl_tag.hpp
    include/ase/player/components/tag/player_mgr_tag.hpp
    include/ase/player/components/tag/player_pst_dty_tag.hpp
    include/ase/player/components/tag/player_spnd_tag.hpp
    include/ase/player/components/tag/player_roam_wand_tag.hpp
    include/ase/player/components/tag/player_roam_rest_tag.hpp
    include/ase/player/components/tag/player_roam_run_tag.hpp

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
    include/ase/player/systems/hub/player_hub_spnd_sys.hpp
    include/ase/player/systems/hub/player_hub_roam_sys.hpp
    include/ase/player/systems/hub/player_hub_sess_reg_sys.hpp
    include/ase/player/systems/hub/player_hub_sess_strt_sys.hpp

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
    include/ase/player/systems/lifecycle/player_life_desp_sys.hpp
    include/ase/player/systems/lifecycle/player_life_roam_ini_sys.hpp
    include/ase/player/systems/lifecycle/player_life_spwn_sys.hpp
    include/ase/player/systems/migration/player_mig_aply_sys.hpp
    include/ase/player/systems/migration/player_mig_des_sys.hpp
    include/ase/player/systems/migration/player_mig_ser_sys.hpp

    #[[
    HEADER FILES - SYSTEMS (reception/)
    Backend-driven spawn: drain the Replica-forwarded BIN_MSG_PLAYER_SPAWN=77 (LANE_SPW) into a
    PlayerReqSpwnComponent (Phase 13 Task 13.10).
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
    include/ase/player/systems/simulation/player_sim_tlpt_sys.hpp
    include/ase/player/systems/simulation/player_sim_wand_sys.hpp
    include/ase/player/systems/simulation/player_sim_rest_sys.hpp
    include/ase/player/systems/simulation/player_sim_wrld_sys.hpp

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
    include/ase/player/systems/sync/player_sync_tlpt_sys.hpp

    #[[
    SOURCE FILES - RECEPTION
    Backend-driven spawn: drain BIN_MSG_PLAYER_SPAWN=77 (LANE_SPW) → PlayerReqSpwnComponent (Task 13.10).
    ]]
    src/reception/player_spwn_rcv_sys.cpp

    #[[
    SOURCE FILES - LIFECYCLE
    Player spawn and despawn management.
    ]]
    src/lifecycle/player_life_desp_sys.cpp
    src/lifecycle/player_life_roam_ini_sys.cpp
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
    src/simulation/player_sim_tlpt_sys.cpp
    src/simulation/player_sim_wand_sys.cpp
    src/simulation/player_sim_rest_sys.cpp
    src/simulation/player_sim_wrld_sys.cpp

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
    Publish the session index register so consumers can discover live players (Hub has no
    iteration API): PLR_ACTIVE_COUNT + PLR_OWNER per slot.
    ]]
    src/hub/player_hub_pos_sys.cpp
    src/hub/player_hub_spnd_sys.cpp
    src/hub/player_hub_roam_sys.cpp
    src/hub/player_hub_sess_reg_sys.cpp
    src/hub/player_hub_sess_strt_sys.cpp

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
    Both halves of the seam in the owner's hand: the serializer bumps the monotonic epoch and lays
    the frozen 42-byte image as hub row HUB_PLR_MIG_STG; PlayerMigDesSystem takes the incoming rows
    HUB_PLR_MIG_DLV and HUB_PLR_MIG_ACK into bridge components, PlayerMigAplySystem builds the figure
    from them with its own types. The World tier only sends and folds frames.
    ]]
    src/migration/player_mig_aply_sys.cpp
    src/migration/player_mig_des_sys.cpp
    src/migration/player_mig_ser_sys.cpp

    #[[
    SOURCE FILES - SYNC
    Input synchronization from network.
    ]]
    src/sync/player_sync_inp_sys.cpp
    src/sync/player_sync_tlpt_sys.cpp

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

#[[
RUNTIME PROFILE (the form of ase-genetics; the Vivarium device carries this module as the writer of
the PLR_POS_* values ase-terrain reads and the HUB_PLR_SPND rows ase-bdi reads, D1494)
server   (default): ase_player.module, the manifest goes beside it (ase_install_module_toml).
embedded:           libase_player.so on Linux as on Android; the client stages the manifest into
                    its bundle (clients/avt-client-android/cmake/AvtRuntime.cmake), not this build.
The server properties above stand FIRST: scripts/generate_toml_manifests.sh reads the first
occurrence in this file.
]]
if(DEFINED ASE_RUNTIME_PROFILE AND NOT ASE_RUNTIME_PROFILE MATCHES "^(server|embedded)$")
    message(FATAL_ERROR "ase-player: ASE_RUNTIME_PROFILE must be server or embedded, got '${ASE_RUNTIME_PROFILE}'")
endif()
if(ASE_RUNTIME_PROFILE STREQUAL "embedded")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
else()
    ase_install_module_toml(${PROJECT_NAME})
endif()

# Header files are already tracked via add_library() above
# This ensures rebuilds when component/system registrations change

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

if(ASE_RUNTIME_PROFILE STREQUAL "embedded")
    #[[
    NO SECOND STATIC COPY: ECS and log are carried by the one runtime libase_kernel_embedded.so
    (kernel/ase-kernel/embedded/CMakeLists.txt). ase::ecs and ase::log do not exist in this profile.
    ]]
    target_link_libraries(${PROJECT_NAME}
        PUBLIC
            ase::kernel-embedded  # Layer 2 - the one runtime: kernel, ECS, log
            ase::math             # Layer 0 - Vec3, math utilities
            ase::types            # Layer 0 - Type definitions (sentinel checks)
        PRIVATE
            ase::hub              # Layer 3 - Star Topology (write positions for L4 plugins)
            ase::transport        # Layer 1 - ctx-shared inbound FIFO lanes (PlayerSpwnRcvSystem drains LANE_SPW)
    )
else()
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ase::ecs          # Layer 1 - ECS framework
        ase::log::headers          # Layer 1 - Logging
        ase::math         # Layer 0 - Vec3, math utilities
        ase::types        # Layer 0 - Type definitions (sentinel checks)
        # No ase::input, ase::terrain or ase::lifecycle: the life mark of the observer adoption comes
        # as row kind HUB_LIFE_ALIV over the star.
    PRIVATE
        ase::hub          # Layer 3 - Star Topology (write positions for L4 plugins)
        ase::transport    # Layer 1 - ctx-shared inbound FIFO lanes (PlayerSpwnRcvSystem drains LANE_SPW; websocket-FREE, L3→L1 allowed)
)
endif()
#[[ VERBOTEN: ase::replication, ase::network, ase::serial, nlohmann_json! Hub handles I/O! ]]

# The module tests build in both profiles. In the embedded profile the test reaches ECS and log only
# through the one runtime libase_kernel_embedded.so the module links, so its program holds no second
# static copy; the Vivarium client builds and runs them in its own tree.
if(ASE_BUILD_TESTS)
    add_subdirectory(tests)
endif()

include(GNUInstallDirs)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
