find_package(Protobuf)
if(NOT PROTOBUF_FOUND)
  message("Global Protobuf is not found, used Grpc embedded protobuf")
  set(PROTOBUF_SRC_ROOT_FOLDER ${GRPC_SRC_ROOT}/third_party/protobuf)

  set(ORG_CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH})
  set(ORG_CMAKE_PROGRAM_PATH ${CMAKE_PROGRAM_PATH})

  set(CMAKE_LIBRARY_PATH ${GRPC_SRC_ROOT}/libs/opt/protobuf ${CMAKE_LIBRARY_PATH})
  set(CMAKE_PROGRAM_PATH ${GRPC_SRC_ROOT}/bins/opt/protobuf ${CMAKE_PROGRAM_PATH})

  find_package(Protobuf)

  set(CMAKE_LIBRARY_PATH ${ORG_CMAKE_LIBRARY_PATH})
  set(CMAKE_PROGRAM_PATH ${ORG_CMAKE_PROGRAM_PATH})
endif()

if(NOT PROTOBUF_FOUND)
  message(SEND_ERROR "Protobuf is required to use grpc")
  return()
endif()

if(GRPC_SRC_ROOT)
  set(GRPC_INCLUDE_SEARCH_PATH "${GRPC_SRC_ROOT}/include")
  set(GRPC_LIBRARY_SEARCH_PATH "${GRPC_SRC_ROOT}/libs/opt")
endif()

find_path(GRPC_INCLUDE_DIR grpc/grpc.h PATHS ${GRPC_INCLUDE_SEARCH_PATH})

find_library(GPR_LIBRARY gpr PATHS ${GRPC_LIBRARY_SEARCH_PATH})

find_library(GRPC_LIBRARY grpc PATHS ${GRPC_LIBRARY_SEARCH_PATH})

find_library(GRPC_CPP_LIBRARY grpc++ PATHS ${GRPC_LIBRARY_SEARCH_PATH})

find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin PATHS "${GRPC_SRC_ROOT}/bins/opt")

if(GRPC_JAVA_ROOT)
  find_program(GRPC_JAVA_PLUGIN protoc-gen-grpc-java
               PATHS "${GRPC_JAVA_ROOT}/compiler/build/binaries/java_pluginExecutable")
endif()

if(NOT GRPC_JAVA_PLUGIN)
  message("grpc java generator plugin not found")
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GRPC GRPC_INCLUDE_DIR
  GPR_LIBRARY GRPC_LIBRARY GRPC_CPP_LIBRARY GRPC_CPP_PLUGIN)

if (NOT GRPC_FOUND)
  return()
endif()

set(GRPC_INCLUDE_DIRS ${GRPC_INCLUDE_DIR})
set(GRPC_LIBRARIES ${GPR_LIBRARY} ${GRPC_LIBRARY} ${GRPC_CPP_LIBRARY})

include(CMakeParseArguments)

function(PROTOBUF_GENERATE_CPP_WITH_GRPC SRCS HDRS)
  if(NOT ARGN)
    message(SEND_ERROR "Error: PROTOBUF_GENERATE_CPP() called without any proto files")
    return()
  endif()

  if(NOT PROTOBUF_PROTOC_EXECUTABLE)
    message(SEND_ERROR "Error: PROTOBUF_PROTOC_EXECUTABLE not found")
    return()
  endif()

  #  cmake_parse_arguments(PROTOC_ARG "GENERATE_GRPC" "" "" ${ARGN})
  #  set(ARGN ${PROTOC_ARG_UNPARSED_ARGUMENTS})

  if(PROTOBUF_GENERATE_CPP_APPEND_PATH)
    # Create an include path for each file specified
    foreach(FIL ${ARGN})
      get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
      get_filename_component(ABS_PATH ${ABS_FIL} PATH)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
      if(${_contains_already} EQUAL -1)
          list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  else()
    set(_protobuf_include_path -I ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  if(DEFINED PROTOBUF_IMPORT_DIRS)
    foreach(DIR ${PROTOBUF_IMPORT_DIRS})
      get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
      if(${_contains_already} EQUAL -1)
          list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  endif()

  set(${SRCS})
  set(${HDRS})
  foreach(FIL ${ARGN})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)

    list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc")
    list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc")
    list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h")
    list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h")

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc"
             "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc"
             "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h"
             "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h"
      COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --cpp_out  ${CMAKE_CURRENT_BINARY_DIR}
           --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
           --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
           ${_protobuf_include_path} ${ABS_FIL}
      DEPENDS ${ABS_FIL} ${PROTOBUF_PROTOC_EXECUTABLE}
      COMMENT "Running C++ protocol buffer compiler on ${FIL}"
      VERBATIM )
  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()


