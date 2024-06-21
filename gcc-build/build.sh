#!/bin/bash

#don't even think about building for linux with this
mkdir build
g++ ../freesprite/*.cpp ../freesprite/libpng/*.c ../freesprite/pugixml/*.cpp ../freesprite/zip/*.c ../freesprite/easybmp/*.cpp -std=c++20 -mwindows -Wl,-Bstatic -lpthread -lz -lmingw32 -Wl,-Bdynamic -ld3d9 -ldwmapi -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -static-libstdc++ -static-libgcc -o ./build/voidsprite.exe
