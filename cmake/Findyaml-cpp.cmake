# Findyaml-cpp.cmake
# Find yaml-cpp library

# Try to find yaml-cpp using pkg-config
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_yaml_cpp yaml-cpp IMPORTED_TARGET)
    if(PC_yaml_cpp_FOUND)
        set(yaml-cpp_FOUND TRUE)
        set(yaml-cpp_VERSION ${PC_yaml_cpp_VERSION})
        add_library(yaml-cpp::yaml-cpp ALIAS PkgConfig::PC_yaml_cpp)
    endif()
endif()

# Fallback to manual search if pkg-config didn't find it
if(NOT yaml-cpp_FOUND)
    find_path(yaml-cpp_INCLUDE_DIR
        NAMES yaml-cpp/yaml.h
        PATHS
            /usr/include
            /usr/local/include
            /opt/local/include
            ${CMAKE_PREFIX_PATH}
            ${CMAKE_INSTALL_PREFIX}
        PATH_SUFFIXES
            include
    )

    find_library(yaml-cpp_LIBRARY
        NAMES yaml-cpp libyaml-cpp
        PATHS
            /usr/lib
            /usr/local/lib
            /opt/local/lib
            ${CMAKE_PREFIX_PATH}
            ${CMAKE_INSTALL_PREFIX}
        PATH_SUFFIXES
            lib
            lib64
    )

    if(yaml-cpp_INCLUDE_DIR AND yaml-cpp_LIBRARY)
        set(yaml-cpp_FOUND TRUE)
        add_library(yaml-cpp::yaml-cpp UNKNOWN IMPORTED)
        set_target_properties(yaml-cpp::yaml-cpp PROPERTIES
            IMPORTED_LOCATION "${yaml-cpp_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${yaml-cpp_INCLUDE_DIR}"
        )
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(yaml-cpp
    REQUIRED_VARS yaml-cpp_LIBRARY yaml-cpp_INCLUDE_DIR
    VERSION_VAR yaml-cpp_VERSION
)

mark_as_advanced(yaml-cpp_INCLUDE_DIR yaml-cpp_LIBRARY)
