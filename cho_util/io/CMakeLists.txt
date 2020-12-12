# Main viewer core
add_library(
  cho_util_io SHARED
  include/cho_util/io/io_fwd.hpp include/cho_util/io/io.hpp
  include/cho_util/io/pipe_io_fwd.hpp include/cho_util/io/pipe_io.hpp
  include/cho_util/io/queue_io_fwd.hpp include/cho_util/io/queue_io.hpp
  )

target_include_directories(
  cho_util_io PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
add_library(${PROJECT_NAME}::io ALIAS cho_util_io)

set_target_properties(cho_util_io PROPERTIES EXPORT_NAME io)
set_target_properties(cho_util_io PROPERTIES LINKER_LANGUAGE CXX)

generate_export_header(cho_util_io)

install(
  TARGETS cho_util_io
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING
  PATTERN "*.hpp")
