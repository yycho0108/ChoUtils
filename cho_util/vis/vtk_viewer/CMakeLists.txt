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

# Main viewer core
add_library(
  cho_util_vtk_viewer SHARED
  include/cho_util/vis/vtk_viewer/vtk_viewer_fwd.hpp
  include/cho_util/vis/vtk_viewer/vtk_viewer.hpp
  include/cho_util/vis/vtk_viewer/handlers.hpp
  include/cho_util/vis/vtk_viewer/handler/cloud_handler.hpp
  include/cho_util/vis/vtk_viewer/handler/cuboid_handler.hpp
  include/cho_util/vis/vtk_viewer/handler/cylinder_handler.hpp
  include/cho_util/vis/vtk_viewer/handler/line_handler.hpp
  include/cho_util/vis/vtk_viewer/handler/plane_handler.hpp
  include/cho_util/vis/vtk_viewer/handler/point_handler.hpp
  include/cho_util/vis/vtk_viewer/handler/sphere_handler.hpp
  include/cho_util/vis/vtk_viewer/handler/point_pair_line_filter.hpp
  include/cho_util/vis/vtk_viewer/handler/poly_point_source.hpp
  include/cho_util/vis/vtk_viewer/handler/color_filter.hpp
  src/point_pair_line_filter.cpp
  src/poly_point_source.cpp
  src/color_filter.cpp
  src/vtk_viewer.cpp)

target_include_directories(
  cho_util_vtk_viewer
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(cho_util_vtk_viewer PRIVATE ${VTK_LIBRARIES}
                                                  cho_util_vis_common)
target_link_libraries(cho_util_vtk_viewer PUBLIC ChoUtil::core ChoUtil::util
                                                 ChoUtil::io fmt)
vtk_module_autoinit(TARGETS cho_util_vtk_viewer MODULES ${VTK_LIBRARIES})

# alias ...
add_library(${PROJECT_NAME}::vtk_viewer ALIAS cho_util_vtk_viewer)

# install ...
install(
  TARGETS cho_util_vtk_viewer
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING
  PATTERN "*.hpp")
