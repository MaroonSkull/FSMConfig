cmake_minimum_required(VERSION 3.15)

# ============================================================================
# Version Information
# ============================================================================
set(FSMCONFIG_VERSION_MAJOR 1)
set(FSMCONFIG_VERSION_MINOR 0)
set(FSMCONFIG_VERSION_PATCH 0)
set(FSMCONFIG_VERSION_PRERELEASE "alpha.1")
set(FSMCONFIG_VERSION "${FSMCONFIG_VERSION_MAJOR}.${FSMCONFIG_VERSION_MINOR}.${FSMCONFIG_VERSION_PATCH}-${FSMCONFIG_VERSION_PRERELEASE}")
set(FSMCONFIG_VERSION_FULL "${FSMCONFIG_VERSION}")

project(FSMConfig VERSION ${FSMCONFIG_VERSION_MAJOR}.${FSMCONFIG_VERSION_MINOR}.${FSMCONFIG_VERSION_PATCH} LANGUAGES CXX)

# ============================================================================
# C++ Standard and Compiler Options
# ============================================================================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings as errors
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# ============================================================================
# Build Options
# ============================================================================
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# ============================================================================
# Dependencies
# ============================================================================
# Version pinning for reproducible builds
# To update dependency versions, modify the version constraints below.
# Ensure compatibility testing after version updates.
#
# Current pinned versions:
# - yaml-cpp: 0.7.0 or higher (required for YAML parsing)
# - GTest: 1.14.0 or higher (required for unit testing)

# Try to find yaml-cpp using standard CMake CONFIG first (for system packages)
find_package(yaml-cpp 0.7.0 QUIET CONFIG)
if(NOT yaml-cpp_FOUND)
    # Fallback to PkgConfig (for Arch Linux and other systems)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(yaml-cpp 0.7.0 IMPORTED_TARGET yaml-cpp)
    endif()
endif()
if(NOT yaml-cpp_FOUND)
    # Last resort: use custom Findyaml-cpp.cmake module
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    find_package(yaml-cpp 0.7.0 REQUIRED)
endif()

# ============================================================================
# Include Directories
# ============================================================================
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# ============================================================================
# Version Header Configuration
# ============================================================================
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/fsmconfig/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/fsmconfig/version.hpp"
    @ONLY
)

# Add the generated header to the include path
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

# ============================================================================
# Subdirectories
# ============================================================================
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ============================================================================
# Installation
# ============================================================================
install(DIRECTORY include/fsmconfig DESTINATION include)
install(TARGETS fsmconfig
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# ============================================================================
# CMake Package Configuration
# ============================================================================
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/fsmconfig-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fsmconfig-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/fsmconfig-config.cmake"
    INSTALL_DESTINATION lib/cmake/fsmconfig
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/fsmconfig-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/fsmconfig-config-version.cmake"
    DESTINATION lib/cmake/fsmconfig
)

# ============================================================================
# UML Diagram Generation
# ============================================================================
find_program(CLANG_UML clang-uml)
if(CLANG_UML)
    add_custom_target(uml-diagrams
        COMMAND ${CLANG_UML} --config ${CMAKE_CURRENT_SOURCE_DIR}/.clang-uml -g mermaid
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating UML diagrams with clang-uml (Mermaid format)..."
        VERBATIM
    )
    
    message(STATUS "clang-uml found: UML diagram generation available via 'make uml-diagrams'")
else()
    message(STATUS "clang-uml not found: UML diagram generation disabled")
endif()
