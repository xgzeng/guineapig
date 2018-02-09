include (CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Generic)

set(ARMGCC_TOOLCHAIN_DIR "/mnt/d/Cypress/gcc-arm-none-eabi-7-2017-q4-major-linux")

if(NOT IS_DIRECTORY ${ARMGCC_TOOLCHAIN_DIR})
  message(FATAL_ERROR "ARMGCC_TOOLCHAIN_DIR ${ARMGCC_TOOLCHAIN_DIR} not exists")
endif()

CMAKE_FORCE_C_COMPILER(${ARMGCC_TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc gnu)
CMAKE_FORCE_CXX_COMPILER(${ARMGCC_TOOLCHAIN_DIR}/bin/arm-none-eabi-g++ gnu)
