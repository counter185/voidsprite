#!/usr/bin/env sh
mkdir AppDir
mkdir AppDir/usr
mkdir AppDir/usr/bin
mkdir AppDir/usr/bin/assets
mkdir AppDir/usr/share
mkdir AppDir/usr/share/icons
cp build/src/assets/icon_voidsprite32x32.png AppDir/usr/share/icons/vsp-icon.png
cp build/src/*.ttf AppDir/usr/bin
cp build/src/assets/* AppDir/usr/bin/assets
cp build/src/voidsprite AppDir/usr/bin/voidsprite
mkdir /tmp/appimage-libs
cp ./build/SDL_image/libSDL3_image.so.0 /tmp/appimage-libs/libSDL3_image.so.0
cp ./build/SDL_net/libSDL3_net.so.0 /tmp/appimage-libs/libSDL3_net.so.0
cp ./build/SDL_ttf/libSDL3_ttf.so.0 /tmp/appimage-libs/libSDL3_ttf.so.0
cp ./build/fmt/libfmt.so.12 /tmp/appimage-libs/libfmt.so.12
cp ./build/liblcf/libliblcf.so /tmp/appimage-libs/libliblcf.so
cp ./build/zlib/libz.so.1 /tmp/appimage-libs/libz.so.1
appimage-builder --skip-test
rm -rf /tmp/appimage-libs