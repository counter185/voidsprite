 CXX = g++-13
 CPPFLAGS = -I../freesprite/liblcf -I../freesprite/liblcf/generated -std=c++20 -Wl,-rpath,. -Wl,-Bstatic -lpthread -lz -lpng -Wl,-Bdynamic -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -static-libstdc++ -static-libgcc
 CPPFILES = ../freesprite/*.cpp ../freesprite/pugixml/*.cpp ../freesprite/zip/*.c ../freesprite/easybmp/*.cpp ../freesprite/astc_dec/*.cpp
 CPPOUTNAME = ./build/voidsprite.x86_64

build: make_build_dir unzip_devlibs copy_assets
	$(CXX) $(CPPFILES) $(CPPFLAGS) -o $(CPPOUTNAME)

make_build_dir:
	mkdir -p build

unzip_devlibs:
	unzip -q -n devlibs_linux.zip *.a -d .
	unzip -q -n devlibs_linux.zip *.so.* -d build
	rm build/libSDL2-2.0.so.0
	#FOR NOW UNTIL I FIGURE OUT WHAT ITS PROBLEM IS

copy_assets:
	cp -r ../freesprite/assets ./build/
	cp ../freesprite/*.ttf ./build/
