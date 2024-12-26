![voidsprite logo](freesprite/assets/mainlogo.png)

# voidsprite

Free C++ sprite editor

![Preview image](img_preview_1.png)

## Supported file formats
### Import:
- PNG
- BMP
- JPEG XL
- OpenRaster ORA
- RPGMaker 2000/2003 XYZ
- Atrophy Engine AETEX (tga/dds)
- Wii TPL*
- DDS* (DXT1, DXT2/3, DXT4/5)
- Cave Story engine PBM
- VTF* (BGRA, BGR, DXT1, DXT2/3, DXT4/5)
- NES (dumps CHR-ROM)
- Mario Paint save file (SRM)
- SR8 
- PXM Cave Story Map File
- LMU RPGMaker2000/3 Map File
- all other formats supported by SDL2_Image

\* - not all subformats are currently implemented   

### Export:
- PNG
- JPEG XL
- OpenRaster ORA
- RPGMaker 2000/2003 XYZ
- BMP
- Cave Story engine PBM
- PXM Cave Story Map File
- PSP Pixel Studio Project File
- C header (as `uint8_t` array)
- Python file (as [R,G,B,A] `np.array`)
- HTML document (inline Base64 image)

## Installing

You can grab a pre-built binary from the latest CI artifacts: https://nightly.link/counter185/voidsprite/workflows/msbuild/main

For Windows, get the `voidsprite-build.zip` file.

For Linux, get the `voidsprite-build-linux-flatpak-x86_64.zip` file; this requires [Flatpak](https://flatpak.org) to be installed. You can also try the portable binary from `voidsprite-build-linux.zip`, but this might not work on all systems.

## Building

### Windows

1. Go to `freesprite/devlibs` and extract `devlibs.7z` to the current directory (right click `devlibs.7z` > `Extract here`).
2. Go back to the repository root and run the `copy_devlibs.bat` script
3. Open `freesprite.sln` with Visual Studio and build/run like any other C++ program

### Linux

1. Install the dependencies and their respective development packages: sdl2, sdl2_ttf, sdl2_image, libpng, pugixml, zlib, liblcf, libjxl, libhwy
   * Some dependencies (libpng, pugixml, zlib, liblcf) will be automatically downloaded and built during the build step if not installed - don't worry if your distro does not package them.
   * GCC/G++ 13 or later is required
2. Install meson (note that version 0.62.2 or higher is required; Ubuntu 22.04 users will want to install a newer version directly from pip with `pip3 install --user meson`)
3. Run `./linux_build.sh`
   * You can also pass the `--run` flag to automatically run the built binary, the `--global` flag to install to `/usr/local`, and `--portable` to generate a portable build (assets stored in the same directory as the executable).
   * If you installed a separate version of GCC to use alongside your system install, you can specify a different compiler by setting the `CC` and `CXX` variables while calling `linux_build.sh`, e.g. `CC=gcc-13 CXX=g++-13 ./linux_build.sh`

By default, the generated output files will be placed in `/tmp/voidsprite`. When `--global` is passed, they will be installed to `/usr/local` instead.

#### Flatpak

You can also build the program as a Flatpak, see [Flatpak README](https://github.com/counter185/voidsprite/blob/main/freesprite/linux/flatpak/README.md).

## Community

You may find the [Official Discord server](https://discord.gg/c5SndMJKj2) here.

[Itch.io page](https://cntrpl.itch.io/voidsprite)

[BlueSky page](https://voidsprite.bsky.social/)


## Licenses

voidsprite is licensed under GPLv2.

Licenses for other libraries and fonts used in voidsprite can be found in the `OPEN_SOURCE_LICENSES` directory
