#!/bin/bash

mkdir build
g++-13 ../freesprite/*.cpp ../freesprite/pugixml/*.cpp ../freesprite/zip/*.c ../freesprite/easybmp/*.cpp ../freesprite/astc_dec/*.cpp  -std=c++20 -Wl,-Bstatic -lpthread -lz -lpng -Wl,-Bdynamic -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -static-libstdc++ -static-libgcc -o ./build/voidsprite
