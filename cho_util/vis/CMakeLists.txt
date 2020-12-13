find_package(Boost REQUIRED COMPONENTS iostreams serialization)
find_package(Eigen3 REQUIRED)
find_package(fmt)

# Datatype Conversion for proto <-> geometry types
add_library(cho_util_vis_convert_proto SHARED
            include/cho_util/vis/convert_proto.hpp src/convert_proto.cpp)
target_link_libraries(
  cho_util_vis_convert_proto PUBLIC ChoUtil::core ChoUtil::util ChoUtil::type
                                    ChoUtil::proto)
target_include_directories(
  cho_util_vis_convert_proto
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Common core ...
add_library(
  cho_util_vis_common SHARED
  include/cho_util/vis/render_data_fwd.hpp
  include/cho_util/vis/render_data.hpp include/cho_util/vis/event_data.hpp)
target_include_directories(
  cho_util_vis_common
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
set_target_properties(cho_util_vis_common PROPERTIES LINKER_LANGUAGE CXX)

# VTK Viewer ...
add_subdirectory(vtk_viewer)

# FIXME(ycho): Enable imgui viewer.
add_subdirectory(imgui_viewer)

# GRPC Library ...
add_library(
  cho_util_grpc_remote_viewer SHARED
  include/cho_util/vis/remote_viewer_server.hpp
  include/cho_util/vis/remote_viewer_client.hpp src/remote_viewer_client.cpp
  src/remote_viewer_server.cpp src/remote_viewer.cpp)

target_include_directories(
  cho_util_grpc_remote_viewer
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(
  cho_util_grpc_remote_viewer
  PUBLIC ChoUtil::proto ChoUtil::core ChoUtil::vtk_viewer ChoUtil::type
         cho_util_vis_convert_proto ChoUtil::io)
add_library(${PROJECT_NAME}::grpc_remote_viewer ALIAS
            cho_util_grpc_remote_viewer)

# Grpc Remote Viewer Server App.
add_executable(grpc_remote_viewer_server_app src/remote_viewer_server_app.cpp)
target_include_directories(
  grpc_remote_viewer_server_app
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(grpc_remote_viewer_server_app
                      PRIVATE cho_util_grpc_remote_viewer ChoUtil::proto)

# Main vis library ...
add_library(
  cho_util_vis SHARED
  src/direct_viewer.cpp
  src/subprocess_viewer.cpp
  src/subprocess.cpp
  include/cho_util/vis/render_data_fwd.hpp
  include/cho_util/vis/render_data.hpp
  include/cho_util/vis/subprocess.hpp
  include/cho_util/vis/subprocess_viewer.hpp
  include/cho_util/vis/direct_viewer.hpp)

set_target_properties(cho_util_vis PROPERTIES EXPORT_NAME vis)

target_include_directories(
  cho_util_vis PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_include_directories(
  cho_util_vis PRIVATE ${Boost_INCLUDE_DIRS}
                       $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(cho_util_vis PRIVATE fmt Boost::iostreams
                                           Boost::serialization)

# Adding this alias is only useful internally.
add_library(${PROJECT_NAME}::vis ALIAS cho_util_vis)
target_link_libraries(
  cho_util_vis PUBLIC ChoUtil::core ChoUtil::vtk_viewer ChoUtil::type
                      ChoUtil::io ChoUtil::grpc_remote_viewer)

generate_export_header(cho_util_vis)

install(
  TARGETS cho_util_vis cho_util_vis_common cho_util_grpc_remote_viewer
          grpc_remote_viewer_server_app cho_util_vis_convert_proto
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING
  PATTERN "*.hpp")
