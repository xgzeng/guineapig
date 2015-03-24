project(grpc_example)
cmake_minimum_required(VERSION 3.2)

find_package(Protobuf)

set(CMAKE_BUILD_TYPE DEBUG)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(GRPC_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../grpc)
find_package(GRpc)
if(NOT GRPC_FOUND)
  message(FATAL_ERROR "grpc is required to build")
endif()

if(NOT PROTOBUF_FOUND)
  message(FATAL_ERROR "Protobuf is required to build")
endif()

protobuf_generate_cpp_with_grpc(PROTO_SRCS PROTO_HDRS hello.proto)

include_directories(${GRPC_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_FLAGS "-std=c++11")

add_library(hello ${PROTO_SRCS} ${PROTO_HDRS})

add_executable(hello_server Server.cpp)

target_link_libraries(hello_server hello
    ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES})

add_executable(hello_client Client.cpp)
target_link_libraries(hello_client hello
    ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES})

