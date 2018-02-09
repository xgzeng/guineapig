cmake_minimum_required(VERSION 3.5)

add_executable(elf2img elf2img.c)

install(TARGETS elf2img
  DESTINATION bin
)