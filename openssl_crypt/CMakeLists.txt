project(grpc_example)
cmake_minimum_required(VERSION 3.2)

if(APPLE)
  set(OPENSSL_ROOT_DIR /usr/local/opt/openssl/)
endif()

find_package(OpenSSL)
if(NOT OpenSSL_FOUND)
  message(FATAL_ERROR "OpenSSL library not found")
endif()

link_libraries(OpenSSL::Crypto)
add_executable(rsa_crypt rsa_crypt.cpp)
