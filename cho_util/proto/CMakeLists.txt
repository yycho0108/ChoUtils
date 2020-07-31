cmake_minimum_required(VERSION 3.12)

find_package(Threads REQUIRED)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

generate_proto(
  "${CMAKE_CURRENT_BINARY_DIR}/cho_util/proto/" ProtoGrpcSources
  ProtoGrpcHeaders "${CMAKE_CURRENT_SOURCE_DIR}/render.proto"
  "${CMAKE_CURRENT_SOURCE_DIR}/geometry.proto")

message("Gen ${ProtoGrpcSources} ${ProtoGrpcHeaders}")

add_library(cho_util_proto SHARED ${ProtoGrpcSources} ${ProtoGrpcHeaders})
set_target_properties(cho_util_proto PROPERTIES EXPORT_NAME proto)

target_link_libraries(
  cho_util_proto PUBLIC ${_REFLECTION} ${_PROTOBUF_LIBPROTOBUF}
                        ${_GRPC_GRPCPP})

target_include_directories(
  cho_util_proto PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_include_directories(
  cho_util_proto PRIVATE ${VTK_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

# Adding this alias is only useful internally.
add_library(${PROJECT_NAME}::proto ALIAS cho_util_proto)

generate_export_header(cho_util_proto)

install(
  TARGETS cho_util_proto
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Protobuf headers
install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cho_util/proto/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/cho_util/proto/
  FILES_MATCHING
  PATTERN "*.pb.h")

# Regular header files
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING
  PATTERN "*.hpp")
