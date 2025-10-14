cmake_minimum_required(VERSION 3.6)

project(my_app)

option(USE_FMT_FORMAT "Use fmt::format instead of std::format" OFF)
set(VOIDSPRITE_ASSETS_PATH "" CACHE STRING "Path to voidsprite assets. Executable directory if empty")

if (NOT USE_FMT_FORMAT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
    add_definitions(-DUSE_FMT_FORMAT=1)
    if(NOT TARGET fmt)
        find_package(fmt CONFIG)
    endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -fdiagnostics-show-option -w -Wno-c++11-narrowing -g")


add_definitions(-DVOIDSPRITE_JXL_ENABLED=0)
if(DEFINED ENV{GIT_HASH})
    add_definitions(-DGIT_HASH="$ENV{GIT_HASH}")
endif()
if(DEFINED ENV{GIT_BRANCH})
    add_definitions(-DGIT_BRANCH="$ENV{GIT_BRANCH}")
endif()

if(NOT TARGET SDL3::SDL3)
    find_package(SDL3 CONFIG)
endif()
if(NOT TARGET SDL3_ttf::SDL3_ttf)
    find_package(SDL3_ttf CONFIG)
endif()
if(NOT TARGET SDL3_image::SDL3_image)
    find_package(SDL3_image CONFIG)
endif()

if(NOT TARGET SDL3_net::SDL3_net)
    find_package(SDL3_net CONFIG)
endif()

if(NOT TARGET liblcf)
    find_package(liblcf CONFIG)
endif()
if(NOT TARGET zlib)
    find_package(zlib CONFIG)
endif()

if(NOT TARGET SDL3::SDL3)
    find_library(SDL3_LIBRARY NAMES "SDL3")
    find_path(SDL3_INCLUDE_DIR NAMES "SDL3/SDL.h")
    add_library(SDL3::SDL3 UNKNOWN IMPORTED)
    set_property(TARGET SDL3::SDL3 PROPERTY IMPORTED_LOCATION "${SDL3_LIBRARY}")
    set_property(TARGET SDL3::SDL3 PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SDL3_INCLUDE_DIR}")
endif()
if(NOT TARGET SDL3_ttf::SDL3_ttf)
    find_library(SDL3_TTF_LIBRARY NAMES "SDL3_ttf")
    find_path(SDL3_TTF_INCLUDE_DIR NAMES "SDL3_ttf/SDL_ttf.h")
    add_library(SDL3_ttf::SDL3_ttf UNKNOWN IMPORTED)
    set_property(TARGET SDL3_ttf::SDL3_ttf PROPERTY IMPORTED_LOCATION "${SDL3_TTF_LIBRARY}")
    set_property(TARGET SDL3_ttf::SDL3_ttf PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SDL3_TTF_INCLUDE_DIR}")
endif()
if(NOT TARGET SDL3_image::SDL3_image)
    find_library(SDL3_IMAGE_LIBRARY NAMES "SDL3_image")
    find_path(SDL3_IMAGE_INCLUDE_DIR NAMES "SDL3_image/SDL_image.h")
    add_library(SDL3_image::SDL3_image UNKNOWN IMPORTED)
    set_property(TARGET SDL3_image::SDL3_image PROPERTY IMPORTED_LOCATION "${SDL3_IMAGE_LIBRARY}")
    set_property(TARGET SDL3_image::SDL3_image PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SDL3_IMAGE_INCLUDE_DIR}")
endif()
if(NOT TARGET SDL3_net::SDL3_net)
    find_library(SDL3_IMAGE_LIBRARY NAMES "SDL3_net")
    find_path(SDL3_IMAGE_INCLUDE_DIR NAMES "SDL3_net/SDL3_net.h")
    add_library(SDL3_net::SDL3_net UNKNOWN IMPORTED)
    set_property(TARGET SDL3_net::SDL3_net PROPERTY IMPORTED_LOCATION "${SDL3_NET_LIBRARY}")
    set_property(TARGET SDL3_net::SDL3_net PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SDL3_NET_INCLUDE_DIR}")
endif()

if(NOT TARGET SDL3::SDL3)
    message(FATAL_ERROR "Cannot find SDL3.

Possible ways to fix this:
- Use a SDL3 Android aar archive, and configure gradle to use it: prefab is required.
- Add add_subdirectory(path/to/SDL) to your CMake script, and make sure a vendored SDL is present there.
")
endif()

file(COPY voidsprite/assets DESTINATION "${CMAKE_BINARY_DIR}/src")
file(COPY voidsprite/appfont-MPLUSRounded1c-Medium.ttf DESTINATION "${CMAKE_BINARY_DIR}/src")
file(COPY voidsprite/appfontcyr-ZenKakuGothicNew-Medium.ttf DESTINATION "${CMAKE_BINARY_DIR}/src")
file(COPY voidsprite/appfontjp-NotoSansJP-Medium.ttf DESTINATION "${CMAKE_BINARY_DIR}/src")

file(GLOB_RECURSE CPP_FILES "voidsprite/*.cpp")
file(GLOB_RECURSE C_FILES "voidsprite/*.c")

if(NOT VOIDSPRITE_ASSETS_PATH STREQUAL "")
    add_compile_definitions(VOIDSPRITE_ASSETS_PATH="${VOIDSPRITE_ASSETS_PATH}/")
endif()

add_executable(voidsprite
        ${CPP_FILES}
        ${C_FILES}
)
set_source_files_properties(${C_FILES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION true)
set_property(TARGET main PROPERTY UNITY_BUILD true)
set_property(TARGET main PROPERTY UNITY_BUILD_MODE BATCH)
set_property(TARGET main PROPERTY UNITY_BUILD_BATCH_SIZE 200)

target_link_libraries(voidsprite PRIVATE SDL3::SDL3)
target_link_libraries(voidsprite PRIVATE SDL3_ttf::SDL3_ttf)
target_link_libraries(voidsprite PRIVATE SDL3_image::SDL3_image)
target_link_libraries(voidsprite PRIVATE SDL3_net::SDL3_net)
target_link_libraries(voidsprite PRIVATE liblcf)
target_link_libraries(voidsprite PRIVATE ZLIB::ZLIB)
if (USE_FMT_FORMAT)
    target_link_libraries(voidsprite PRIVATE fmt)
endif()
