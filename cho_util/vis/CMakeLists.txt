find_package(Boost REQUIRED COMPONENTS iostreams serialization)
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

add_library(
  cho_util_vis SHARED
  src/direct_viewer.cpp
  src/subprocess_viewer.cpp
  src/handlers.cpp
  src/subprocess.cpp
  include/cho_util/vis/io_fwd.hpp
  include/cho_util/vis/pipe_io_fwd.hpp
  include/cho_util/vis/queue_io_fwd.hpp
  include/cho_util/vis/render_data_fwd.hpp
  include/cho_util/vis/handler_fwd.hpp
  include/cho_util/vis/vtk_viewer_fwd.hpp
  include/cho_util/vis/io.hpp
  include/cho_util/vis/pipe_io.hpp
  include/cho_util/vis/queue_io.hpp
  include/cho_util/vis/render_data.hpp
  include/cho_util/vis/handlers.hpp
  include/cho_util/vis/vtk_viewer.hpp
  include/cho_util/vis/subprocess.hpp
  include/cho_util/vis/subprocess_viewer.hpp)

set_target_properties(cho_util_vis PROPERTIES EXPORT_NAME vis)

target_include_directories(
  cho_util_vis PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_include_directories(
  cho_util_vis PRIVATE ${VTK_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                       $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(
  cho_util_vis PRIVATE ${VTK_LIBRARIES} fmt Boost::iostreams
                       Boost::serialization ChoUtil::core)
vtk_module_autoinit(TARGETS cho_util_vis MODULES ${VTK_LIBRARIES})

# Adding this alias is only useful internally.
add_library(${PROJECT_NAME}::vis ALIAS cho_util_vis)

generate_export_header(cho_util_vis)
install(
  TARGETS cho_util_vis
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING
  PATTERN "*.hpp")
