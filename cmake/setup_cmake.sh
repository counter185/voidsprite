#!/usr/bin/env sh
cd "$(dirname "$(realpath "$0")")"
if [ ! -d SDL ]; then git clone --recursive https://github.com/libsdl-org/SDL.git; fi
if [ ! -d SDL_image ]; then git clone --recursive https://github.com/libsdl-org/SDL_image.git; fi
if [ ! -d SDL_ttf ]; then git clone --recursive https://github.com/libsdl-org/SDL_ttf.git; fi
if [ ! -d SDL_net ]; then git clone --recursive https://github.com/libsdl-org/SDL_net.git; fi
if [ ! -d zlib ]; then git clone --recursive https://github.com/madler/zlib.git; fi
if [ ! -d fmt ]; then git clone --recursive https://github.com/fmtlib/fmt.git; fi
if [ ! -d liblcf ]; then ln -s "../external_liblcf" liblcf; fi
cd src
if [ ! -d voidsprite ]; then ln -s "../../freesprite" voidsprite; fi
