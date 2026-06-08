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
