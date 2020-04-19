function(generate_proto OUTPUT_DIR SRCS HDRS)
  if(NOT ARGN)
    message(
      SEND_ERROR
        "Error: PROTOBUF_GENERATE_GRPC_CPP() called without any proto files")
    return()
  endif()

  if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
  else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
  endif()

  set(_GRPC_GRPCPP gRPC::grpc++)
  if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
  else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
  endif()

  file(MAKE_DIRECTORY ${OUTPUT_DIR})

  set(${SRCS})
  set(${HDRS})
  foreach(FIL ${ARGN})
    get_filename_component(proto ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    get_filename_component(proto_path ${FIL} PATH)

    list(APPEND ${SRCS} "${OUTPUT_DIR}/${FIL_WE}.grpc.pb.cc")
    list(APPEND ${HDRS} "${OUTPUT_DIR}/${FIL_WE}.grpc.pb.h")
    list(APPEND ${SRCS} "${OUTPUT_DIR}/${FIL_WE}.pb.cc")
    list(APPEND ${HDRS} "${OUTPUT_DIR}/${FIL_WE}.pb.h")

    add_custom_command(
      OUTPUT "${OUTPUT_DIR}/${FIL_WE}.grpc.pb.cc"
             "${OUTPUT_DIR}/${FIL_WE}.grpc.pb.h"
             "${OUTPUT_DIR}/${FIL_WE}.pb.cc"
             "${OUTPUT_DIR}/${FIL_WE}.pb.h"
      COMMAND
        ${_PROTOBUF_PROTOC} ARGS --grpc_out ${OUTPUT_DIR}
        --cpp_out ${OUTPUT_DIR} -I "${proto_path}"
        --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE} "${proto}"
      DEPENDS ${proto}
      COMMENT "Running gRPC C++ protocol buffer compiler on ${FIL}"
      VERBATIM)
  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS}
      ${${SRCS}}
      PARENT_SCOPE)
  set(${HDRS}
      ${${HDRS}}
      PARENT_SCOPE)
endfunction()
