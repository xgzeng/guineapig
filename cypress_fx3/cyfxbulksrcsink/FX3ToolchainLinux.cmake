include (CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Generic)

set(ARMGCC_TOOLCHAIN_DIR "/mnt/d/Cypress/gcc-arm-none-eabi-7-2017-q4-major-linux")

if(NOT IS_DIRECTORY ${ARMGCC_TOOLCHAIN_DIR})
  message(FATAL_ERROR "ARMGCC_TOOLCHAIN_DIR ${ARMGCC_TOOLCHAIN_DIR} not exists")
endif()

#set(CMAKE_SYSROOT ${IMX6_TOOLCHAIN_DIR}/arm-imx6-linux-gnueabi/sysroot)
#set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

CMAKE_FORCE_C_COMPILER(${ARMGCC_TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc gnu)
CMAKE_FORCE_CXX_COMPILER(${ARMGCC_TOOLCHAIN_DIR}/bin/arm-none-eabi-g++ gnu)

# arm-none-eabi-gcc -g -DTX_ENABLE_EVENT_TRACE -DDEBUG -DCYU3P_FX3=1 -D__CYU3P_TX__=1 
# -I. -I../../../fw_lib/1_3_3/inc  -O0 -Wall -mcpu=arm926ej-s -mthumb-interwork -ffunction-sections -fdata-sections -c -o cyfxbulksrcsink.o cyfxbulksrcsink.c

#set(CMAKE_C_COMPILER ${CYFX3_TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc)
#set(CMAKE_CXX_COMPILER ${CYFX3_TOOLCHAIN_DIR}/bin/arm-none-eabi-g++)
