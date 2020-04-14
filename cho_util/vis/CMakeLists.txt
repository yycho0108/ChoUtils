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
  vis SHARED
  src/embedded_viewer.cpp
  src/handlers.cpp
  src/subprocess.cpp
  include/cho_util/vis/handler_fwd.hpp
  include/cho_util/vis/pipe_io_fwd.hpp
  include/cho_util/vis/render_data_fwd.hpp
  include/cho_util/vis/embedded_viewer.hpp
  include/cho_util/vis/handlers.hpp
  include/cho_util/vis/pipe_io.hpp
  include/cho_util/vis/render_data.hpp
  include/cho_util/vis/subprocess.hpp)

target_include_directories(
  vis PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_include_directories(
  vis PRIVATE ${VTK_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(vis PRIVATE ${VTK_LIBRARIES} fmt Boost::iostreams
                                  Boost::serialization)
vtk_module_autoinit(TARGETS vis MODULES ${VTK_LIBRARIES})

message("Adding ${PROJECT_NAME}::vis target")
add_library(${PROJECT_NAME}::vis ALIAS vis)

generate_export_header(vis)
install(
  TARGETS vis
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
