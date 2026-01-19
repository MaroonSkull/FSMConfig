cmake_minimum_required(VERSION 3.15)
project(FSMConfig VERSION 0.1.0 LANGUAGES CXX)

# ============================================================================
# C++ Standard and Compiler Options
# ============================================================================
set(CMAKE_CXX_STANDARD 26)
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
# Try to find yaml-cpp using standard CMake CONFIG first (for system packages)
find_package(yaml-cpp QUIET CONFIG)
if(NOT yaml-cpp_FOUND)
    # Fallback to PkgConfig (for Arch Linux and other systems)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(yaml-cpp IMPORTED_TARGET yaml-cpp)
    endif()
endif()
if(NOT yaml-cpp_FOUND)
    # Last resort: use custom Findyaml-cpp.cmake module
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    find_package(yaml-cpp REQUIRED)
endif()

# ============================================================================
# Include Directories
# ============================================================================
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

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
