cmake_minimum_required(VERSION 3.5)

project(cyfxbulksrcsink C ASM)

# Build Host Utility Tools
include(ExternalProject)
ExternalProject_Add(cyfx3_host_tools
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tools"
  INSTALL_COMMAND ""  # Skip install step
)

ExternalProject_Get_Property(cyfx3_host_tools BINARY_DIR)
#message(FATAL_ERROR ${BINARY_DIR})
set(CYFX3_ELF2IMG_EXECUTALBE "${BINARY_DIR}/elf2img")
set(BINARY_DIR)

# 
set(CYFX3_SDK_DIR "/mnt/d/projects/env/FX3_SDK/cyfx3sdk")
if(NOT IS_DIRECTORY ${CYFX3_SDK_DIR})
  message(FATAL_ERROR "CYFX3_SDK_DIR ${CYFX3_SDK_DIR} not exists")
endif()

set(CYFX3_FW_INCLUDE_DIR "${CYFX3_SDK_DIR}/fw_lib/1_3_3/inc")
set(CYFX3_FW_LIB_DIR "${CYFX3_SDK_DIR}/fw_lib/1_3_3/fx3_debug")

#add_compile_options(-mcpu=arm926ej-s -mthumb-interwork
#  --entry CyU3PFirmwareEntry
#  -Wno-wchar-size-warning
#  #-nodefaultlibs -nostdlib
#)
set(FX3_COMPILE_FLAGS "-mcpu=arm926ej-s -mthumb-interwork")
string(CONCAT FX3_LINK_FLAGS
    " -Wl,--no-wchar-size-warning"
    " -Wl,--entry=CyU3PFirmwareEntry"
    " -Wl,--gc-sections"
    " -Wl,-T${CYFX3_SDK_DIR}/fw_build/fx3_fw/fx3_512k.ld")

#set(CMAKE_C_FLAGS "${FX3_LINK_FLAGS}")

add_definitions(-DCYU3P_FX3 -D__CYU3P_TX__=1)

link_directories(${CYFX3_FW_LIB_DIR})

add_executable(cyfxbulksrcsink
    cyfx_gcc_startup.S
    cyfxbulkdscr.c
    cyfxbulksrcsink.c
    cyfxbulksrcsink.h
    cyfxtx.c
)

# target_compile_definitions(cyfxbulksrcsink
#   PRIVATE
#     -D__CYU3P_TX__=1
# )

target_include_directories(cyfxbulksrcsink
  PRIVATE
    ${CYFX3_FW_INCLUDE_DIR}
)

target_link_libraries(cyfxbulksrcsink
  cyu3sport cyu3lpp cyfxapi cyu3threadx
)

set_target_properties(cyfxbulksrcsink
  PROPERTIES
    OUTPUT_NAME cyfxbulksrcsink.elf
    COMPILE_FLAGS ${FX3_COMPILE_FLAGS}
    LINK_FLAGS ${FX3_LINK_FLAGS}
)

# convert output .elf file to .img
add_custom_command(OUTPUT cyfxbulksrcsink.img
  COMMAND ${CYFX3_ELF2IMG_EXECUTALBE} -i cyfxbulksrcsink.elf -o cyfxbulksrcsink.img
)
