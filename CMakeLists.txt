cmake_minimum_required(VERSION 3.14)
project(Cell)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(SDL2Test ${SDL2_INCLUDE_DIRS})

add_executable(Cell main.cpp engine/Engine.cpp engine/Engine.h engine/cell/Cell.cpp engine/cell/Cell.h)
target_link_libraries(Cell ${SDL2_LIBRARIES})
