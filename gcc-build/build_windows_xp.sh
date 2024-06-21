g++ ../freesprite/*.cpp ../freesprite/libpng/*.c ../freesprite/pugixml/*.cpp 
../freesprite/zip/*.c ../freesprite/easybmp/*.cpp -DWINDOWS_XP=1 -D_WIN32_WINNT=0x0501 
-std=c++20 -mwindows -Wl,-Bstatic -lpthreads -lz -lmingw32 -Wl,-Bdynamic -ld3d9 -lSDL2main 
-lSDL2 -lSDL2_image -lSDL2_ttf -static-libstdc++ -static-libgcc
