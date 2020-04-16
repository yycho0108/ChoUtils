add_library(cho_util_util src/timer.cpp)
set_target_properties(cho_util_util PROPERTIES EXPORT_NAME util)

target_include_directories(
  cho_util_util PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
add_library(${PROJECT_NAME}::util ALIAS cho_util_util)

generate_export_header(cho_util_util)
install(
  TARGETS cho_util_util
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING
  PATTERN "*.hpp")
