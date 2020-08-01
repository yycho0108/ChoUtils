cmake_minimum_required(VERSION 2.8)
project(choutil-test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Threads - required for ProtoBuf package resolution somehow.
# find_package(Threads REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(ChoUtil REQUIRED PATHS "$ENV{HOME}/Repos/Ravel/ChoUtils/install/")

if(FALSE)
  get_cmake_property(_variableNames VARIABLES)
  list(SORT _variableNames)
  foreach(_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endif()

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${ChoUtil_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ChoUtil::core ChoUtil::util)

add_executable(remote_viewer_client_test remote_viewer_client.cpp)
target_include_directories(remote_viewer_client_test
                           PUBLIC ${ChoUtil_INCLUDE_DIRS})
target_link_libraries(
  remote_viewer_client_test PRIVATE ChoUtil::core ChoUtil::util ChoUtil::vis
                                    ChoUtil::proto Eigen3::Eigen)

add_executable(direct_viewer_client_test direct_viewer_test.cpp)
target_include_directories(direct_viewer_client_test
                           PUBLIC ${ChoUtil_INCLUDE_DIRS})
target_link_libraries(
  direct_viewer_client_test PRIVATE ChoUtil::core ChoUtil::util ChoUtil::vis
                                    ChoUtil::proto Eigen3::Eigen)
