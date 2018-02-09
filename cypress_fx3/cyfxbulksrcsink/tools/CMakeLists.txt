cmake_minimum_required(VERSION 3.5)

add_executable(elf2img elf2img.c)

add_executable(download_fx3 download_fx3.cpp libcyusb.cpp)
target_link_libraries(download_fx3 usb-1.0)
