find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include(FetchContent)

# GL3W FetchContent_Declare( gl3w GIT_REPOSITORY
# https://github.com/skaslev/gl3w.git GIT_TAG master GIT_SHALLOW 1)
# FetchContent_MakeAvailable(gl3w)

# IMGUI
FetchContent_Populate(
  imgui
  GIT_REPOSITORY git@github.com:ocornut/imgui.git
  GIT_TAG docking
  GIT_SHALLOW 1
  SOURCE_DIR imgui)

add_library(
  cho_util_imgui_viewer SHARED
  ${imgui_SOURCE_DIR}/imgui.h
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  include/cho_util/vis/imgui_viewer/imgui_viewer.hpp
  include/glad/glad.h
  include/KHR/khrplatform.h
  src/imgui_viewer.cpp
  src/glad.c)

target_include_directories(
  cho_util_imgui_viewer
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_include_directories(cho_util_imgui_viewer PRIVATE ${imgui_SOURCE_DIR})

target_include_directories(
  cho_util_imgui_viewer
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(cho_util_imgui_viewer PRIVATE glfw OpenGL::GL)
target_link_libraries(cho_util_imgui_viewer PRIVATE cho_util_vis_common)
target_link_libraries(cho_util_imgui_viewer PUBLIC ChoUtil::core ChoUtil::util
                                                   ChoUtil::io fmt)
target_compile_definitions(cho_util_imgui_viewer
                           PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

# alias ...
add_library(${PROJECT_NAME}::imgui_viewer ALIAS cho_util_imgui_viewer)

add_subdirectory(test)

# install ...
install(
  TARGETS cho_util_imgui_viewer
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING
  PATTERN "*.hpp")
