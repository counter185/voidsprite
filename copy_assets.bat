md x64\Debug\assets
md x64\Release\assets
md gcc-build\build\
md gcc-build\build\assets
copy freesprite\*.ttf x64\Debug\
copy freesprite\*.ttf x64\Release\
copy freesprite\*.ttf gcc-build\build
copy freesprite\assets\* x64\Debug\assets\
copy freesprite\assets\* x64\Release\assets\
copy freesprite\assets\* gcc-build\build\assets