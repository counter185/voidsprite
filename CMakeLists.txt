cmake_minimum_required(VERSION 3.15...4.0)
project(voidsprite)

set(VOIDSPRITE_ASSETS_PATH "" CACHE STRING "Specify the assets path")
option(VOIDSPRITE_STATIC_SDL3 "Statically link SDL3" OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D VOIDSPRITE_JXL_ENABLED=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D VOIDSPRITE_ASSETS_PATH=\\\"${VOIDSPRITE_ASSETS_PATH}\\\"")

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
if(VOIDSPRITE_STATIC_SDL3)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/SDL3")
        execute_process(
            COMMAND git clone https://github.com/libsdl-org/SDL --branch release-3.2.10 lib/SDL3 --depth 1
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )
    endif()

    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/SDL3_ttf")
        execute_process(
            COMMAND git clone https://github.com/libsdl-org/SDL_ttf --branch release-3.2.2 lib/SDL3_ttf --depth 1
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )
    endif()

    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/SDL3_image")
        execute_process(
            COMMAND git clone https://github.com/libsdl-org/SDL_image --branch release-3.2.4 lib/SDL3_image --depth 1
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )
    endif()

    set(__VOIDSPRITE_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")
    set(BUILD_SHARED_LIBS OFF)
    set(SDL_STATIC ON)
    set(SDL_SHARED OFF)

    include_directories("${CMAKE_SOURCE_DIR}/lib/SDL3/include")
    include_directories("${CMAKE_SOURCE_DIR}/lib/SDL3_ttf/include")
    include_directories("${CMAKE_SOURCE_DIR}/lib/SDL3_image/include")

    add_subdirectory("${CMAKE_SOURCE_DIR}/lib/SDL3")
    add_subdirectory("${CMAKE_SOURCE_DIR}/lib/SDL3_ttf")
    add_subdirectory("${CMAKE_SOURCE_DIR}/lib/SDL3_image")

    set(BUILD_SHARED_LIBS "${__VOIDSPRITE_BUILD_SHARED_LIBS}")
else()
    find_package(SDL3 REQUIRED)
    find_package(SDL3_ttf REQUIRED)
    find_package(SDL3_image REQUIRED)
endif()

add_executable(voidsprite ${src})
target_link_libraries(voidsprite PUBLIC z)
if(NOT VOIDSPRITE_STATIC_SDL3)
    target_link_libraries(voidsprite PUBLIC SDL3)
    target_link_libraries(voidsprite PUBLIC SDL3_ttf)
    target_link_libraries(voidsprite PUBLIC SDL3_image)
else()
    target_link_libraries(voidsprite PRIVATE SDL3::SDL3-static)
    target_link_libraries(voidsprite PRIVATE SDL3_ttf-static)
    target_link_libraries(voidsprite PRIVATE SDL3_image-static)
endif()
target_link_libraries(voidsprite PUBLIC jxl)
target_link_libraries(voidsprite PRIVATE discord_game_sdk)
target_link_libraries(voidsprite PRIVATE liblcf)
target_link_libraries(voidsprite PRIVATE libpng)

file(COPY freesprite/assets DESTINATION ${CMAKE_BINARY_DIR})
