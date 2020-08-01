find_package(Boost REQUIRED COMPONENTS iostreams serialization)
find_package(Eigen3 REQUIRED)
find_package(fmt)

find_package(
  VTK
  COMPONENTS vtkCommonColor
             vtkCommonCore
             vtkFiltersSources
             vtkFiltersGeneral
             vtkInteractionStyle
             vtkInteractionStyle
             vtkRenderingContextOpenGL2
             vtkRenderingCore
             vtkRenderingFreeType
             vtkRenderingGL2PSOpenGL2
             vtkRenderingOpenGL2
             QUIET)

# Datatype Conversion for proto <-> geometry types
add_library(vis_convert_proto SHARED include/cho_util/vis/convert_proto.hpp
                                     src/convert_proto.cpp)
target_link_libraries(vis_convert_proto PUBLIC ChoUtil::core ChoUtil::util
                                               ChoUtil::type ChoUtil::proto)
target_include_directories(
  vis_convert_proto
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Main viewer core
add_library(
  vtk_viewer SHARED
  include/cho_util/vis/vtk_viewer_fwd.hpp
  include/cho_util/vis/render_data_fwd.hpp
  include/cho_util/vis/vtk_viewer.hpp
  include/cho_util/vis/event_data.hpp
  include/cho_util/vis/handlers.hpp
  include/cho_util/vis/handler/cloud_handler.hpp
  include/cho_util/vis/handler/cuboid_handler.hpp
  include/cho_util/vis/handler/cylinder_handler.hpp
  include/cho_util/vis/handler/line_handler.hpp
  include/cho_util/vis/handler/plane_handler.hpp
  include/cho_util/vis/handler/point_handler.hpp
  include/cho_util/vis/handler/sphere_handler.hpp
  include/cho_util/vis/handler/point_pair_line_filter.hpp
  include/cho_util/vis/render_data.hpp
  src/handlers.cpp
  src/point_pair_line_filter.cpp
  src/vtk_viewer.cpp)

target_include_directories(
  vtk_viewer PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(vtk_viewer PRIVATE ${VTK_LIBRARIES})
target_link_libraries(vtk_viewer PUBLIC ChoUtil::core ChoUtil::util fmt)
vtk_module_autoinit(TARGETS vtk_viewer MODULES ${VTK_LIBRARIES})
add_library(${PROJECT_NAME}::vtk_viewer ALIAS vtk_viewer)

# GRPC Library ...
add_library(
  grpc_remote_viewer SHARED
  include/cho_util/vis/remote_viewer_server.hpp
  include/cho_util/vis/remote_viewer_client.hpp
  src/remote_viewer_client.cpp
  src/remote_viewer_server.cpp
  src/remote_viewer.cpp
  include/cho_util/vis/io_fwd.hpp
  include/cho_util/vis/render_data_fwd.hpp
  include/cho_util/vis/io.hpp
  include/cho_util/vis/queue_io.hpp
  include/cho_util/vis/render_data.hpp)

target_include_directories(
  grpc_remote_viewer
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(
  grpc_remote_viewer PUBLIC ChoUtil::proto ChoUtil::core ChoUtil::vtk_viewer
                            ChoUtil::type vis_convert_proto)
add_library(${PROJECT_NAME}::grpc_remote_viewer ALIAS grpc_remote_viewer)

# Grpc Remote Viewer App...
add_executable(grpc_remote_viewer_server_app src/remote_viewer_server_app.cpp)
target_include_directories(
  grpc_remote_viewer_server_app
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(grpc_remote_viewer_server_app PRIVATE grpc_remote_viewer
                                                            ChoUtil::proto)

# Main vis library ...
add_library(
  cho_util_vis SHARED
  src/direct_viewer.cpp
  src/subprocess_viewer.cpp
  src/subprocess.cpp
  include/cho_util/vis/io_fwd.hpp
  include/cho_util/vis/pipe_io_fwd.hpp
  include/cho_util/vis/queue_io_fwd.hpp
  include/cho_util/vis/render_data_fwd.hpp
  include/cho_util/vis/vtk_viewer_fwd.hpp
  include/cho_util/vis/io.hpp
  include/cho_util/vis/pipe_io.hpp
  include/cho_util/vis/queue_io.hpp
  include/cho_util/vis/render_data.hpp
  include/cho_util/vis/subprocess.hpp
  include/cho_util/vis/subprocess_viewer.hpp)

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
                      ChoUtil::grpc_remote_viewer)

generate_export_header(cho_util_vis)

install(
  TARGETS cho_util_vis vtk_viewer grpc_remote_viewer
          grpc_remote_viewer_server_app vis_convert_proto
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING
  PATTERN "*.hpp")
