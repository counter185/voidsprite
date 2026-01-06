#!/bin/bash

mkdir android-project/app/src/main/assets
cp freesprite/*.ttf android-project/app/src/main/assets/
cp freesprite/assets/* android-project/app/src/main/assets/

cd android-project/app/jni

git clone --recursive https://github.com/libsdl-org/SDL.git
cd SDL
git checkout c61497b
cd ..
git clone --recursive https://github.com/libsdl-org/SDL_image.git
cd SDL_image
git checkout release-3.2.4
cd ..
git clone --recursive https://github.com/libsdl-org/SDL_ttf.git
#git clone --recursive https://github.com/libsdl-org/SDL_net.git
git clone --recursive https://github.com/counter185/SDL_net-android21.git SDL_net
git clone --recursive https://github.com/madler/zlib.git
cd zlib
git checkout v1.3.1.2
cd ..
ln -s "../../../external_liblcf" liblcf 
cd src
ln -s "../../../../freesprite" voidsprite