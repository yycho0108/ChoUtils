find_package(Boost REQUIRED COMPONENTS program_options)
add_executable(test_generate_convex_polygon
               ${CMAKE_CURRENT_SOURCE_DIR}/test_generate_convex_polygon.cpp)

target_include_directories(
  test_generate_convex_polygon
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(test_generate_convex_polygon ChoUtil::core fmt
                      Boost::program_options)
