cmake_minimum_required(VERSION 3.6)

project(voidsprite)
#liblcf changes this to "d" and makes android debug builds break
set(CMAKE_DEBUG_POSTFIX "")

option(FLATPAK "set to ON if building a flatpak" OFF)
option(VSP_NETWORKING "enable SDL_net functionality" ON)

if (EMSCRIPTEN)
    set(VSP_NETWORKING OFF)
endif()
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -Wno-unused-command-line-argument")
    set(SDLIMAGE_AVIF OFF)
    set(SDLIMAGE_VENDORED OFF)
    set(SDLTTF_VENDORED ON)
endif()

#on flatpak we must install these separately as system libraries
#(add to the flatpak yml file)
#also libavif is already included there by default for some reason
if (NOT FLATPAK)
    add_subdirectory(external/SDL)
    add_subdirectory(external/SDL_image)
    add_subdirectory(external/SDL_ttf)

    if (NOT DEFINED EMSCRIPTEN)
        set(LIBLCF_WITH_INI OFF)
        set(LIBLCF_WITH_ICU OFF)
        set(LIBLCF_WITH_XML OFF)
        add_subdirectory(external/liblcf)
    else()
        set(CONFIG_MULTITHREAD OFF)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msimd128")
        set(COMPILE_C_ONLY ON)
        set(BUILD_SHARED_LIBS OFF)
    endif()

    set(AVIF_LIBYUV "LOCAL")
    set(AVIF_CODEC_AOM OFF)
    set(AVIF_CODEC_LIBGAV1 "LOCAL")
    set(AVIF_CODEC_SVT "LOCAL")
    add_subdirectory(external/libavif)
endif()

if (VSP_NETWORKING)
    add_subdirectory(external/SDL_net)
endif()

#these are static and can be used on virtually any platform
set(ZLIB_BUILD_TESTING OFF)
add_subdirectory(external/zlib)
add_subdirectory(external/fmt)

# Your game and its CMakeLists.txt are in a subfolder named "src"
add_subdirectory(cmake)

