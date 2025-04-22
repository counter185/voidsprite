@echo off

rem   we need admin for symlinks for some reason
net session > nul
if not %errorlevel% == 0 goto noadmin

cd /d %~dp0
cd android-project\app\jni
rem   todo: check if git is even present
git clone --recursive https://github.com/libsdl-org/SDL.git
git clone --recursive https://github.com/libsdl-org/SDL_image.git
git clone --recursive https://github.com/libsdl-org/SDL_ttf.git
git clone --recursive https://github.com/madler/zlib.git
mklink /d liblcf "..\..\..\external_liblcf"
cd src
mklink /d voidsprite "..\..\..\..\freesprite"
exit

:noadmin
echo --------------------------
echo Please launch this script with admin privileges
pause