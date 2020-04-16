cmake_minimum_required(VERSION 3.12)

add_library(cho_util_core src/random.cpp src/math.cpp)
set_target_properties(cho_util_core PROPERTIES EXPORT_NAME core)

target_include_directories(
  cho_util_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

add_library(${PROJECT_NAME}::core ALIAS cho_util_core)

export(
  TARGETS cho_util_core
  NAMESPACE ${PROJECT_NAME}::
  FILE ${PROJECT_NAME}Targets.cmake)

generate_export_header(cho_util_core)
install(
  TARGETS cho_util_core
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING
  PATTERN "*.hpp")
