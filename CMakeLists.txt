cmake_minimum_required(VERSION 3.10)
project(pong LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

add_executable(pong src/main.cpp src/game_main.cpp)
target_link_libraries(pong SDL2)
