#[[
ASE TEST CONFIGURATION

@file        tests/CMakeLists.txt
@brief       Test configuration for ase-player
@description Unit and integration tests with doctest framework.

@module      ase-player
@layer       3 (Module)
@created     2025-01-30
@modified    2025-01-30
@version     1.0.0

CMAKE TEST COMPLIANCE

[ ] add_executable() with explicit test file list
[ ] NO file(GLOB ...) for test files!
[ ] target_link_libraries() with module and doctest::doctest
[ ] set_target_properties() with RUNTIME_OUTPUT_DIRECTORY
[ ] add_test() for CTest integration
[ ] Test executable name: ${PROJECT_NAME}-test
]]

add_executable(${PROJECT_NAME}-test
    #[[
    TEST FILES
    Unit and integration test source files.
    ]]
    test_player.cpp
)

target_link_libraries(${PROJECT_NAME}-test
    PRIVATE
        ase::player
        ase::containers     # Layer 0 - Vector/Pair SSOT (player enumeration test)
        doctest::doctest
)

set_target_properties(${PROJECT_NAME}-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_test(NAME ${PROJECT_NAME}-test COMMAND ${PROJECT_NAME}-test)

#[[
MIGRATE-SERIALIZER TEST TARGET (PLAN_ASE_COMPUTE_PHASE_06_WORLD WS-H.4)
Drives the REAL PlayerMigSerSystem: frozen 42-byte PlayerSnap layout from the live player_st_*
components, monotonic per-player epoch bump (the migrate idempotency source), staged-buffer
re-entry block. Standalone doctest executable next to the module test.
]]
add_executable(${PROJECT_NAME}-mig-test
    #[[
    TEST FILES
    Written 2026-08-23. The source list stood EMPTY here since the target was declared:
    nothing was compiled into it, so the linker found no main and the build failed at
    750/832 with "undefined reference to `main'". The target was intentional - its purpose
    is spelled out above - so the file was written, not the target removed.
    ]]
    test_player_mig.cpp
)

target_link_libraries(${PROJECT_NAME}-mig-test
    PRIVATE
        ase::player
        ase::types        # region_wire.hpp contract constants (PlayerSnap offsets)
        # ase-player links ase::hub PRIVATE, so its include paths do NOT propagate to this
        # target. The test reads the staged delivery (hub::HubPlrMigSnapComponent) and sets the
        # arm (hub::HubPlrMigArmComponent) on the figure, so it needs the star headers itself.
        # Added 2026-08-23: without this line the file does not compile - `ase/hub/api.hpp` is
        # not found, which is a missing dependency, not a reason to drop the includes.
        ase::hub
        doctest::doctest
)

set_target_properties(${PROJECT_NAME}-mig-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_test(NAME ${PROJECT_NAME}-mig-test COMMAND ${PROJECT_NAME}-mig-test)
