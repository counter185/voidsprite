#!/usr/bin/env sh
cd "$(dirname "$(realpath "$0")")"
git clone --recursive https://github.com/libsdl-org/SDL.git
git clone --recursive https://github.com/libsdl-org/SDL_image.git
git clone --recursive https://github.com/libsdl-org/SDL_ttf.git
git clone --recursive https://github.com/libsdl-org/SDL_net.git
git clone --recursive https://github.com/madler/zlib.git
git clone --recursive https://github.com/fmtlib/fmt.git
ln -s "../external_liblcf" liblcf
cd src
ln -s "../../freesprite" voidsprite
