# ============================================================================
# Find GoogleTest
# ============================================================================
find_package(GTest REQUIRED)

# ============================================================================
# Test Executables
# ============================================================================
add_executable(test_config_parser test_config_parser.cpp)
target_link_libraries(test_config_parser
    PRIVATE
        fsmconfig
        GTest::gtest
        GTest::gtest_main
)
add_test(NAME test_config_parser COMMAND test_config_parser)

add_executable(test_state_machine test_state_machine.cpp)
target_link_libraries(test_state_machine
    PRIVATE
        fsmconfig
        GTest::gtest
        GTest::gtest_main
)
add_test(NAME test_state_machine COMMAND test_state_machine)

add_executable(test_callback_registry test_callback_registry.cpp)
target_link_libraries(test_callback_registry
    PRIVATE
        fsmconfig
        GTest::gtest
        GTest::gtest_main
)
add_test(NAME test_callback_registry COMMAND test_callback_registry)

add_executable(test_integration test_integration.cpp)
target_link_libraries(test_integration
    PRIVATE
        fsmconfig
        GTest::gtest
        GTest::gtest_main
)
add_test(NAME test_integration COMMAND test_integration)

add_executable(test_types test_types.cpp)
target_link_libraries(test_types
    PRIVATE
        fsmconfig
        GTest::gtest
        GTest::gtest_main
)
add_test(NAME test_types COMMAND test_types)

add_executable(test_variable_manager test_variable_manager.cpp)
target_link_libraries(test_variable_manager
    PRIVATE
        fsmconfig
        GTest::gtest
        GTest::gtest_main
)
add_test(NAME test_variable_manager COMMAND test_variable_manager)
