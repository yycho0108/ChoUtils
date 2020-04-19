cmake_minimum_required(VERSION 3.12)
project(ChoUtil)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Includes ...
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(cmake/ChoUtilVersion.cmake)
include(cmake/GrpcProto.cmake)

include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Individual modules
add_subdirectory(cho_util)

# Packaging + Installation
# set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR}")

set(INCLUDE_INSTALL_DIR "include/")
set(${PROJECT_NAME}_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR}" )
message("? ${INCLUDE_INSTALL_DIR}")
message("? ${${PROJECT_NAME}_INCLUDE_DIRS}")

# Root module
install(
    EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ChoUtil
    NAMESPACE ${PROJECT_NAME}::
    FILE ${PROJECT_NAME}Targets.cmake # Not sure if this is still needed
    )

configure_package_config_file(
    "Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ChoUtilConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ChoUtil
    PATH_VARS
    INCLUDE_INSTALL_DIR
    CMAKE_INSTALL_LIBDIR
    )

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ChoUtilConfigVersion.cmake
    VERSION ${CU_VERSION_MAJOR}.${CU_VERSION_MINOR}.${CU_VERSION_PATCH}
    COMPATIBILITY SameMajorVersion
    )

### Install Config and ConfigVersion files
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/ChoUtilConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ChoUtilConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ChoUtil"
    )
