#!/bin/bash

mkdir android-project/app/src/main/assets
cp freesprite/*.ttf android-project/app/src/main/assets/
cp freesprite/assets/* android-project/app/src/main/assets/

cd android-project/app/jni

git clone --recursive https://github.com/libsdl-org/SDL.git
cd SDL
git checkout 520d73ae
cd ..
git clone --recursive https://github.com/libsdl-org/SDL_image.git
git clone --recursive https://github.com/libsdl-org/SDL_ttf.git
#git clone --recursive https://github.com/libsdl-org/SDL_net.git
git clone --recursive https://github.com/counter185/SDL_net-android21.git SDL_net
git clone --recursive https://github.com/madler/zlib.git
ln -s "../../../external_liblcf" liblcf 
cd src
ln -s "../../../../freesprite" voidsprite