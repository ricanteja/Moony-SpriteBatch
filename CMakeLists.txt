cmake_minimum_required(VERSION 3.3)
project(MOONYSPRITEBATCH)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(RUNTIME_OUTPUT_DIRECTORY "build")

include_directories(
        "C:/mingw-w64/libraries/sfml/include"
        "C:/mingw-w64/libraries/zlib/include"
        "include"
)

link_directories(
        "C:/mingw-w64/libraries/sfml/lib"
        "C:/mingw-w64/libraries/zlib/lib"
)

add_executable(
        TexturePacker
        src/TexturePacker.cpp
        src/TexturePacker.h
        src/Directories.h
)

add_executable(
        Example
        src/Example.cpp
)

target_link_libraries(
        TexturePacker
        libsfml-system.a
        libsfml-window.a
        libsfml-graphics.a
        libzlibstatic.a
)

target_link_libraries(
        Example
        libsfml-system.a
        libsfml-window.a
        libsfml-graphics.a
        libzlibstatic.a
)