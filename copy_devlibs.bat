mkdir x64\Debug
mkdir x64\Release
mkdir Debug
mkdir Release
mkdir gcc-build\build
copy freesprite\devlibs\lib\x64\*.dll x64\Debug
copy freesprite\devlibs\lib\x64\*.dll x64\Release
copy freesprite\devlibs\lib\x86\*.dll Debug
copy freesprite\devlibs\lib\x86\*.dll Release

copy freesprite\devlibs\lib\x86\*.dll gcc-build\build