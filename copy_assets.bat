md x64\Debug\assets
md x64\Release\assets
md ARM64\Debug\assets
md ARM64\Release\assets
md gcc-build\build\
md gcc-build\build\assets
md android-project\app\src\main\assets
copy freesprite\*.ttf x64\Debug\
copy freesprite\*.ttf x64\Release\
copy freesprite\*.ttf ARM64\Debug\
copy freesprite\*.ttf ARM64\Release\
copy freesprite\*.ttf gcc-build\build
copy freesprite\*.ttf android-project\app\src\main\assets
copy freesprite\assets\* x64\Debug\assets\
copy freesprite\assets\* x64\Release\assets\
copy freesprite\assets\* ARM64\Debug\assets\
copy freesprite\assets\* ARM64\Release\assets\
copy freesprite\assets\* gcc-build\build\assets
copy freesprite\assets\* android-project\app\src\main\assets

md x64\Release\OPEN_SOURCE_LICENSES
copy OPEN_SOURCE_LICENSES x64\Release\OPEN_SOURCE_LICENSES

md ARM64\Release\OPEN_SOURCE_LICENSES
copy OPEN_SOURCE_LICENSES ARM64\Release\OPEN_SOURCE_LICENSES