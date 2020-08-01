cmake_minimum_required(VERSION 2.8)
project(choutil-test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_package(Threads REQUIRED)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

# message("$ENV{HOME}/Repos/Ravel/ChoUtils/install/")
find_package(Eigen3 REQUIRED)
find_package(ChoUtil REQUIRED PATHS "$ENV{HOME}/Repos/Ravel/ChoUtils/install/")

get_cmake_property(_variableNames VARIABLES)
list(SORT _variableNames)
foreach(_variableName ${_variableNames})
  message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${ChoUtil_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ChoUtil::core ChoUtil::util)

add_executable(remote_viewer_client_test remote_viewer_client.cpp)
target_include_directories(remote_viewer_client_test
                           PUBLIC ${ChoUtil_INCLUDE_DIRS})
target_link_libraries(
  remote_viewer_client_test PRIVATE ChoUtil::core ChoUtil::util ChoUtil::vis
                                    ChoUtil::proto Eigen3::Eigen)
