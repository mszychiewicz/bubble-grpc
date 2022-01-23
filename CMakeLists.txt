cmake_minimum_required(VERSION 3.5.1)

project(bubble-grpc)

include(common.cmake)

# Proto file
get_filename_component(b_proto "bubble.proto" ABSOLUTE)
get_filename_component(b_proto_path "${b_proto}" PATH)

# Generated sources
set(b_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/bubble.pb.cc")
set(b_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/bubble.pb.h")
set(b_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/bubble.grpc.pb.cc")
set(b_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/bubble.grpc.pb.h")
add_custom_command(
        OUTPUT "${b_proto_srcs}" "${b_proto_hdrs}" "${b_grpc_srcs}" "${b_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${b_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${b_proto}"
        DEPENDS "${b_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# b_grpc_proto
add_library(b_grpc_proto
        ${b_grpc_srcs}
        ${b_grpc_hdrs}
        ${b_proto_srcs}
        ${b_proto_hdrs})
target_link_libraries(b_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})

# Targets sort_[async_](client|server)
foreach(_target
        sort_client sort_server)
    add_executable(${_target} "${_target}.cc")

    target_link_libraries(${_target}
            b_grpc_proto
            ${_REFLECTION}
            ${_GRPC_GRPCPP}
            ${_PROTOBUF_LIBPROTOBUF})
endforeach()
