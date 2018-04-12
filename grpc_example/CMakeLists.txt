project(grpc_example)
cmake_minimum_required(VERSION 3.2)

set(CMAKE_BUILD_TYPE DEBUG)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(GRpc)
if(NOT GRPC_FOUND)
  message(FATAL_ERROR "grpc is required to build")
endif()

protobuf_generate_cpp_with_grpc(PROTO_SRCS PROTO_HDRS hello.proto)

message(${GRPC_INCLUDE_DIRS})
include_directories(
    ${PROTOBUF_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_FLAGS "-std=c++11")

add_library(hello ${PROTO_SRCS} ${PROTO_HDRS})

add_executable(hello_server cpp/Server.cpp)

target_link_libraries(hello_server hello
    ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES})

add_executable(hello_client cpp/Client.cpp)
target_link_libraries(hello_client hello
    ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES})

add_executable(async_hello_server cpp/AsyncServer.cpp)
target_link_libraries(async_hello_server hello
    ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES})

add_executable(hybrid_hello_server cpp/HybridServer.cpp)
target_link_libraries(hybrid_hello_server hello
    ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES})
