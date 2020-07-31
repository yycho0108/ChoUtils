# Main viewer core
add_library(cho_util_type SHARED include/cho_util/type/convert.hpp
                                 src/convert.cpp)

target_include_directories(
  cho_util_type PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
add_library(${PROJECT_NAME}::type ALIAS cho_util_type)
set_target_properties(cho_util_type PROPERTIES EXPORT_NAME type)

generate_export_header(cho_util_type)

install(
  TARGETS cho_util_type
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING
  PATTERN "*.hpp")
