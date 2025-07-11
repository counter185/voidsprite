![voidsprite logo](README_ASSETS/mainlogo.png)

Free C++ pixel art editor

![Preview image](README_ASSETS/img_preview_1.png)

## Supported file formats

◆ The list of supported file formats has been moved to the [web manual](https://counter185.github.io/voidsprite-web-manual/index.html?page=compatibility).

## Installing

◆ You can grab a pre-built binary from the latest CI artifacts: https://nightly.link/counter185/voidsprite/workflows/msbuild/main

For Windows, get the `voidsprite-build-win64.zip` file.

For Linux, get the `voidsprite-build-linux-flatpak-x86_64.zip` file; this requires [Flatpak](https://flatpak.org) to be installed.

## Building

### Windows

1. Run the `setup_windows_devlibs.bat` script
2. Open `freesprite.sln` with Visual Studio and build/run like any other C++ program  
*Only x64 builds work right now, 32-bit Windows targets do not compile

### Linux

1. Install the dependencies and their respective development packages: sdl3, sdl3_ttf, sdl3_image, libpng, pugixml, zlib, liblcf, libjxl, libhwy
   * Some dependencies (libpng, pugixml, zlib, liblcf) will be automatically downloaded and built during the build step if not installed - don't worry if your distro does not package them.
   * GCC/G++ 13 or later is required
2. Install meson (note that version 0.62.2 or higher is required; Ubuntu 22.04 users will want to install a newer version directly from pip with `pip3 install --user meson`)
3. Run `./linux_build.sh`
   * You can also pass the `--run` flag to automatically run the built binary, the `--global` flag to install to `/usr/local`, and `--portable` to generate a portable build (assets stored in the same directory as the executable).
   * If you installed a separate version of GCC to use alongside your system install, you can specify a different compiler by setting the `CC` and `CXX` variables while calling `linux_build.sh`, e.g. `CC=gcc-13 CXX=g++-13 ./linux_build.sh`

By default, the generated output files will be placed in `/tmp/voidsprite`. When `--global` is passed, they will be installed to `/usr/local` instead.

#### Flatpak

You can also build the program as a Flatpak, see [Flatpak README](https://github.com/counter185/voidsprite/blob/main/freesprite/linux/flatpak/README.md).

## System requirements

### Windows/Linux

|  | Minimum | Recommended/Tested compatible |  
|---|:-:|:-:|  
| OS | Windows 7 / Linux with Flatpak | Windows 10 / Ubuntu 24 / Mint 21 |  
| CPU | any x64 CPU | AMD Ryzen 3 3200G / Intel Core i5-2500K |  
| GPU | any GPU with Direct3D 9 / OpenGL 3 | Intel HD Graphics 620 / AMD Radeon Vega 8 |  
| Display | 1280x720 | 1920x1080 |  
| RAM | 2 GB (comfortable minimum) | 8 GB |  
| Input | - | Wintab/Windows Ink-compatible tablet |  

◆Windows: Visual C++ 2019 runtime is required: [Download](https://aka.ms/vs/17/release/vc_redist.x64.exe)

### Android

|  | Minimum | Recommended/Tested compatible |  
|---|:-:|:-:|  
| OS | Android 5 | Android 9 / Android 14 |  
| SOC | any x64 or arm64 CPU | Exynos 8895 / Snapdragon 778G |  
| Display | 1280x720 | 1080p+ tablet |  
| RAM | 2 GB | 6 GB |  
| Input | - | Samsung S Pen |  

◆Some tools currently require a mouse  
◆Compatibility with chromeOS/Chromebooks is unknown  
◆Devices tested: Galaxy Note 8, Galaxy Tab S7 FE  

### macOS
|  | Minimum | Recommended/Tested compatible |  
|---|:-:|:-:|  
| OS | macOS 13 | TBD |  
| CPU | Intel 64-bit | TBD |
| GPU | TBD | TBD |
| Display | 1280x720 | TBD |  

◆Due to lack of hardware we can't test what works and what doesn't.  
◆Have a Mac and want to help us out? Join the Discord server below.

## Community

◆ [Official Discord server](https://discord.gg/c5SndMJKj2)  
◆ [Itch.io page](https://cntrpl.itch.io/voidsprite)  
◆ [BlueSky page](https://voidsprite.bsky.social/)

## Donate

◆ You can support the project by donating on [itch.io](https://cntrpl.itch.io/voidsprite/purchase)

## Licenses

◆ voidsprite is licensed under GPLv2.  
◆ Licenses for other libraries and fonts used in voidsprite can be found in the `OPEN_SOURCE_LICENSES` directory
