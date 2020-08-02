cmake_minimum_required(VERSION 3.12)

find_package(Eigen3 REQUIRED)

add_library(
  cho_util_core
  SHARED
  src/random.cpp
  src/math.cpp
  include/cho_util/core/version.hpp
  include/cho_util/core/serialize_eigen.hpp
  include/cho_util/core/serialize_std_variant.hpp
  include/cho_util/core/math.hpp
  include/cho_util/core/random.hpp
  include/cho_util/core/thread_safe_queue.hpp
  include/cho_util/core/geometry.hpp
  include/cho_util/core/geometry/cuboid.hpp
  include/cho_util/core/geometry/cylinder.hpp
  include/cho_util/core/geometry/geometry_base.hpp
  include/cho_util/core/geometry/line.hpp
  include/cho_util/core/geometry/plane.hpp
  include/cho_util/core/geometry/point.hpp
  include/cho_util/core/geometry/point_cloud.hpp
  include/cho_util/core/geometry/sphere.hpp)

set_target_properties(cho_util_core PROPERTIES EXPORT_NAME core)

target_include_directories(
  cho_util_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
# target_include_directories(cho_util_core INTERFACE ${Eigen3_INCLUDE_DIRS})
target_include_directories(cho_util_core PUBLIC ${Eigen_INCLUDE_DIRS})
target_link_libraries(cho_util_core PUBLIC Eigen3::Eigen)
# target_link_libraries(cho_util_core LINK_INTERFACE_LIBRARIES Eigen3::Eigen)

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
  PATTERN "*.hpp"
  PATTERN "*/*.hpp")
