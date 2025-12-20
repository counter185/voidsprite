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
endif()

add_subdirectory(zlib)
add_subdirectory(fmt)

# Your game and its CMakeLists.txt are in a subfolder named "src"
add_subdirectory(src)

