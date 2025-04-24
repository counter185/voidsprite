cmake_minimum_required(VERSION 3.15...4.0)

project(
    libpng
    VERSION 1.0
    LANGUAGES C)

file(GLOB_RECURSE src *.c)

add_library(libpng STATIC ${src})
target_sources(libpng PRIVATE ${src})
