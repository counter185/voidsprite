cmake_minimum_required(VERSION 3.15...4.0)
project(voidsprite)

set(VOIDSPRITE_ASSETS_PATH "" CACHE STRING "Specify the assets path")
option(VOIDSPRITE_STATIC_SDL3 "Statically link SDL3" OFF)
option(VOIDSPRITE_JXL_ENABLED "Enable JXL with libjxl" ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

message("")
message("Voidsprite:")
message("- build-sdl3: ${VOIDSPRITE_STATIC_SDL3}")
message("- build-libjxl: ${VOIDSPRITE_JXL_ENABLED}")
message("")

if(VOIDSPRITE_JXL_ENABLED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D VOIDSPRITE_JXL_ENABLED=1")
    set(JPEGXL_STATIC ON)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/libjxl")
        execute_process(
            COMMAND git clone https://github.com/libjxl/libjxl lib/libjxl --depth 1 --branch v0.11.1 --recursive
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )
    endif()
    add_subdirectory("${CMAKE_SOURCE_DIR}/lib/libjxl")
    include_directories("${CMAKE_SOURCE_DIR}/lib/libjxl/lib/include")
    target_link_libraries(voidsprite PRIVATE jxl)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D VOIDSPRITE_JXL_ENABLED=0")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D VOIDSPRITE_ASSETS_PATH=\\\"${VOIDSPRITE_ASSETS_PATH}\\\"")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED On)

list(APPEND CMAKE_PREFIX_PATH "lib")
include_directories(lib)

file(GLOB_RECURSE src freesprite/*.cpp freesprite/*.c)

add_subdirectory("${CMAKE_SOURCE_DIR}/lib/liblcf")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/libpng")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/discord_game_sdk")
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/zlib")
    execute_process(
        COMMAND git clone https://github.com/madler/zlib lib/zlib --depth 1 --branch v1.3.1
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/zlib")
include_directories("${CMAKE_SOURCE_DIR}/lib/zlib")
if(VOIDSPRITE_STATIC_SDL3)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/SDL3")
        execute_process(
            COMMAND git clone https://github.com/libsdl-org/SDL --branch release-3.2.10 lib/SDL3 --depth 1 --recursive
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )
    endif()

    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/SDL3_ttf")
        execute_process(
            COMMAND git clone https://github.com/libsdl-org/SDL_ttf --branch release-3.2.2 lib/SDL3_ttf --depth 1 --recursive
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )
    endif()

    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/SDL3_image")
        execute_process(
            COMMAND git clone https://github.com/libsdl-org/SDL_image --branch release-3.2.4 lib/SDL3_image --depth 1 --recursive
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
target_link_libraries(voidsprite PRIVATE zlibstatic)
target_link_libraries(voidsprite PRIVATE discord_game_sdk)
target_link_libraries(voidsprite PRIVATE liblcf)
target_link_libraries(voidsprite PRIVATE libpng::libpng-static)

file(COPY freesprite/assets DESTINATION ${CMAKE_BINARY_DIR})
