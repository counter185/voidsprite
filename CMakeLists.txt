cmake_minimum_required(VERSION 3.6)

option(VITASDK_BUILD "set to ON if building for vita" OFF)
option(FLATPAK "set to ON if building a flatpak" OFF)
option(VSP_NETWORKING "enable SDL_net functionality" ON)

if(VITASDK_BUILD)
    if(DEFINED ENV{VITASDK})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    else()
        message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
    endif()
    include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)
    set(VSP_NETWORKING OFF)
endif()

project(voidsprite)
#liblcf changes this to "d" and makes android debug builds break
set(CMAKE_DEBUG_POSTFIX "")


if (EMSCRIPTEN)
    set(VSP_NETWORKING OFF)
endif()
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -Wno-unused-command-line-argument")
    set(SDLIMAGE_AVIF OFF)
    set(SDLIMAGE_VENDORED OFF)
    set(SDLTTF_VENDORED ON)
endif()
if (VITASDK)
    set(SDLIMAGE_VENDORED OFF)
    set(SDLIMAGE_AVIF OFF)
    set(SDLIMAGE_TIF OFF)
    set(SDLIMAGE_WEBP OFF)
    set(SDLIMAGE_PNG OFF)
endif()

#on flatpak we must install these separately as system libraries
#(add to the flatpak yml file)
#also libavif is already included there by default for some reason
if (NOT FLATPAK)
    set(SDL_TESTS OFF)
    set(SDL_TEST_LIBRARY OFF)
    set(SDL_EXAMPLES OFF)
    add_subdirectory(external/SDL)
    add_subdirectory(external/SDL_image)
    add_subdirectory(external/SDL_ttf)
    if (NOT VITASDK)

    endif()

    if (NOT DEFINED EMSCRIPTEN)
        set(LIBLCF_WITH_INI OFF)
        set(LIBLCF_WITH_ICU OFF)
        set(LIBLCF_WITH_XML OFF)
        set(LIBLCF_MAIN_PROJECT OFF)
        add_subdirectory(external/liblcf)
    else()
        set(CONFIG_MULTITHREAD OFF)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msimd128")
        set(COMPILE_C_ONLY ON)
        set(BUILD_SHARED_LIBS OFF)
    endif()

    if (NOT VITASDK)
        set(AVIF_LIBYUV "LOCAL")
        set(AVIF_CODEC_AOM OFF)
        set(AVIF_CODEC_LIBGAV1 "LOCAL")
        set(AVIF_CODEC_SVT "LOCAL")
        add_subdirectory(external/libavif)
    endif()
endif()

if (VSP_NETWORKING)
    add_subdirectory(external/SDL_net)
endif()

#these are static and can be used on virtually any platform
set(ZLIB_BUILD_TESTING OFF)
set(ZLIB_BUILD_SHARED OFF)
add_subdirectory(external/zlib)
add_subdirectory(external/fmt)

# Your game and its CMakeLists.txt are in a subfolder named "src"
add_subdirectory(cmake)

