project(sdl2_example)
cmake_minimum_required(VERSION 3.17)

find_package(SDL2 REQUIRED)

add_executable(sdl2_example main.cpp)

target_include_directories(sdl2_example
  PRIVATE
    "/usr/local/include/SDL2"
)

target_link_libraries(sdl2_example SDL2::SDL2)
