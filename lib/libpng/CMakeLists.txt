cmake_minimum_required(VERSION 3.15...4.0)

project(
    libpng
    VERSION 1.0
    LANGUAGES C)

file(GLOB_RECURSE src *.c)

add_library(libpng-static STATIC)
add_library(libpng::libpng-static ALIAS libpng-static)
target_sources(libpng-static PRIVATE ${src})
