cmake_minimum_required(VERSION 3.6)

project(GAME)

# SDL sources are in a subfolder named "SDL"
add_subdirectory(SDL)

# Compilation of companion libraries
add_subdirectory(SDL_image)
#add_subdirectory(SDL_mixer)
add_subdirectory(SDL_ttf)

if (NOT DEFINED EMSCRIPTEN)
    add_subdirectory(SDL_net)
    add_subdirectory(liblcf)
else()
    set(CONFIG_MULTITHREAD OFF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msimd128")
    set(COMPILE_C_ONLY ON)
endif()

add_subdirectory(zlib)
add_subdirectory(fmt)

set(AVIF_LIBYUV "LOCAL")
set(AVIF_CODEC_AOM OFF)
set(AVIF_CODEC_LIBGAV1 "LOCAL")
set(AVIF_CODEC_SVT "LOCAL")
add_subdirectory(libavif)

# Your game and its CMakeLists.txt are in a subfolder named "src"
add_subdirectory(src)

