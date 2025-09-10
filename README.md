![voidsprite logo](README_ASSETS/mainlogo.png)

Free C++ pixel art editor

![Preview image](README_ASSETS/img_preview_1.png)

## ◆Supported file formats

◆ The list of supported file formats has been moved to the [web manual](https://counter185.github.io/voidsprite-web-manual/index.html?page=compatibility).

## ◆Installing

◆ You can grab a pre-built binary from the latest CI artifacts: https://nightly.link/counter185/voidsprite/workflows/msbuild/main

For Windows, get the `voidsprite-build-win64.zip` file.

For Linux, get the `voidsprite-build-linux-flatpak-x86_64.zip` file; this requires [Flatpak](https://flatpak.org) to be installed.

## ◆Building

### Windows

1. Run the `setup_windows_devlibs.bat` script
2. Open `freesprite.sln` with Visual Studio and build/run like any other C++ program  
*Only x64 builds work right now, 32-bit Windows targets do not compile

### Linux

1. Install the dependencies and their respective development packages: sdl3, sdl3_ttf, sdl3_image, sdl3_net, libpng, pugixml, zlib, liblcf, libjxl, libhwy
   * Some dependencies (libpng, pugixml, zlib, liblcf) will be automatically downloaded and built during the build step if not installed - don't worry if your distro does not package them.
   * GCC/G++ 13 or later is required
2. Install meson (note that version 0.62.2 or higher is required; Ubuntu 22.04 users will want to install a newer version directly from pip with `pip3 install --user meson`)
3. Run `./linux_build.sh`
   * You can also pass the `--run` flag to automatically run the built binary, the `--global` flag to install to `/usr/local`, and `--portable` to generate a portable build (assets stored in the same directory as the executable).
   * If you installed a separate version of GCC to use alongside your system install, you can specify a different compiler by setting the `CC` and `CXX` variables while calling `linux_build.sh`, e.g. `CC=gcc-13 CXX=g++-13 ./linux_build.sh`

By default, the generated output files will be placed in `/tmp/voidsprite`. When `--global` is passed, they will be installed to `/usr/local` instead.

#### Flatpak

You can also build the program as a Flatpak, see [Flatpak README](https://github.com/counter185/voidsprite/blob/main/freesprite/linux/flatpak/README.md).

### Optional dependencies

Some dependencies can be disabled with build flags:
- `-DVOIDSPRITE_JXL_ENABLED=0` will disable JPEG XL support (drops `libjxl`, `libhwy` requirement). JPEG XL might still be importable through SDL_image.
- `-DVSP_NETWORKING=0` will disable all socket-based network features, like network canvas (drops `sdl3_net` requirement). HTTP features like update checks and downloading from Lospec are separate and will still work.
- `-DUSE_FMT_FORMAT=1` will use `fmt` for string formatting instead of `std::format`. This drops the requirement for C++20 support, but `fmt` needs to be present.

## ◆System requirements

◆ The system requirements table has been moved to the [web manual](https://counter185.github.io/voidsprite-web-manual/index.html?page=requirements).

## ◆Community

◆ [Official Discord server](https://discord.gg/c5SndMJKj2)  
◆ [Itch.io page](https://cntrpl.itch.io/voidsprite)  
◆ [BlueSky page](https://voidsprite.bsky.social/)

## ◆Contribute

voidsprite is open for community contributions. Here are some ways you can help:

- Open PRs to contribute to the code
- Open Issues (or report them on Discord) to help get bugs and other problems fixed
- Share your custom [Patterns](community-patterns) and [Templates](community-templates)
- Translate voidsprite into your language. [More details](freesprite/localization)

## ◆Donate

◆ You can support the project by donating on [itch.io](https://cntrpl.itch.io/voidsprite/purchase)

## ◆Licenses

◆ voidsprite is licensed under GPLv2.  
◆ Licenses for other libraries and fonts used in voidsprite can be found in the `OPEN_SOURCE_LICENSES` directory
