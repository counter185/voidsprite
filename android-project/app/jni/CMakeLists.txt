cmake_minimum_required(VERSION 3.6)

project(GAME)

# SDL sources are in a subfolder named "SDL"
add_subdirectory(SDL)

# Compilation of companion libraries
add_subdirectory(SDL_image)
#add_subdirectory(SDL_mixer)
add_subdirectory(SDL_ttf)

add_subdirectory(liblcf)

add_subdirectory(zlib)

# Your game and its CMakeLists.txt are in a subfolder named "src"
add_subdirectory(src)

