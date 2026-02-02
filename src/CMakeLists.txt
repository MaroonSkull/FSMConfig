# ============================================================================
# FSMConfig Library
# ============================================================================
add_library(fsmconfig
    fsmconfig/types.cpp
    fsmconfig/state_machine.cpp
    fsmconfig/config_parser.cpp
    fsmconfig/callback_registry.cpp
    fsmconfig/event_dispatcher.cpp
    fsmconfig/state.cpp
    fsmconfig/variable_manager.cpp
)

# Set library version properties
set_target_properties(fsmconfig PROPERTIES
    VERSION ${FSMCONFIG_VERSION}
    SOVERSION ${FSMCONFIG_VERSION_MAJOR}
    EXPORT_NAME fsmconfig
)

target_include_directories(fsmconfig
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(fsmconfig
    PUBLIC
        yaml-cpp::yaml-cpp
)

# ============================================================================
# Installation
# ============================================================================
install(TARGETS fsmconfig
    EXPORT fsmconfig-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(EXPORT fsmconfig-targets
    FILE fsmconfig-targets.cmake
    NAMESPACE fsmconfig::
    DESTINATION lib/cmake/fsmconfig
)
