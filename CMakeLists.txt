cmake_minimum_required(VERSION 3.15...4.0)
project(voidsprite)

set(assets "" CACHE STRING "Specify an assets path")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVOIDSPRITE_JXL_ENABLED=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVOIDSPRITE_ASSETS_PATH=${assets}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED On)

list(APPEND CMAKE_PREFIX_PATH "lib")
include_directories(lib)

file(GLOB_RECURSE src freesprite/*.cpp freesprite/*.c)

find_package(liblcf REQUIRED)
find_package(libpng REQUIRED)
find_package(discord_game_sdk REQUIRED)
find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_image REQUIRED)

add_executable(voidsprite ${src})
target_link_libraries(voidsprite PUBLIC z)
target_link_libraries(voidsprite PUBLIC SDL3)
target_link_libraries(voidsprite PUBLIC SDL3_ttf)
target_link_libraries(voidsprite PUBLIC SDL3_image)
target_link_libraries(voidsprite PUBLIC jxl)
target_link_libraries(voidsprite PRIVATE discord_game_sdk)
target_link_libraries(voidsprite PRIVATE liblcf)
target_link_libraries(voidsprite PRIVATE libpng)

file(COPY freesprite/assets DESTINATION ${CMAKE_BINARY_DIR})
