cmake_minimum_required(VERSION 3.6)

project(GAME)

set(CMAKE_DEBUG_POSTFIX "")

# SDL sources are in a subfolder named "SDL"
add_subdirectory(SDL)

# Compilation of companion libraries
add_subdirectory(SDL_image)
#add_subdirectory(SDL_mixer)
add_subdirectory(SDL_ttf)
add_subdirectory(SDL_net)

set(LIBLCF_WITH_INI OFF)
set(LIBLCF_WITH_ICU OFF)
set(LIBLCF_WITH_XML OFF)
add_subdirectory(liblcf)

add_subdirectory(zlib)

set(AVIF_LIBYUV "LOCAL")
set(AVIF_CODEC_AOM OFF)
set(AVIF_CODEC_LIBGAV1 "LOCAL")
set(AVIF_CODEC_SVT "LOCAL")
if(CMAKE_ANDROID_ARCH_ABI MATCHES "^x86")
    set(COMPILE_C_ONLY ON)
endif()
#we need to pretend to be a mac so that svt-av1 doesn't try to grab non-existent exes from clang
set(APPLE ON)
add_subdirectory(libavif)

# Your game and its CMakeLists.txt are in a subfolder named "src"
add_subdirectory(src)

