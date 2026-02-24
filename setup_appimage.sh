#!/usr/bin/env sh
mkdir AppDir
mkdir AppDir/usr
mkdir AppDir/usr/bin
mkdir AppDir/usr/bin/assets
mkdir AppDir/usr/share
mkdir AppDir/usr/share/icons
cp build_appimage/cmake/assets/icon_voidsprite32x32.png AppDir/usr/share/icons/vsp-icon.png
cp build_appimage/cmake/*.ttf AppDir/usr/bin
cp build_appimage/cmake/assets/* AppDir/usr/bin/assets
cp build_appimage/cmake/voidsprite AppDir/usr/bin/voidsprite
mkdir /tmp/appimage-libs
cp ./cmake/libdiscord_game_sdk_linux_x64.so /tmp/appimage-libs/libdiscord_game_sdk_linux_x64.so
cp ./build_appimage/external/SDL_image/libSDL3_image.so.0 /tmp/appimage-libs/libSDL3_image.so.0
cp ./build_appimage/external/SDL_net/libSDL3_net.so.0 /tmp/appimage-libs/libSDL3_net.so.0
cp ./build_appimage/external/SDL_ttf/libSDL3_ttf.so.0 /tmp/appimage-libs/libSDL3_ttf.so.0
cp ./build_appimage/external/fmt/libfmt.so.12 /tmp/appimage-libs/libfmt.so.12
cp ./build_appimage/external/liblcf/liblcf.so.0 /tmp/appimage-libs/liblcf.so.0
cp ./build_appimage/external/zlib/libz.so.1 /tmp/appimage-libs/libz.so.1
cp ./build_appimage/external/libavif/libavif.so.16 /tmp/appimage-libs/libavif.so.16
appimage-builder --skip-test
rm -rf /tmp/appimage-libs
