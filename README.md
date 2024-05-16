![voidsprite logo](freesprite/assets/mainlogo.png)

# voidsprite

Free C++ sprite editor

![Preview image](img_preview_1.png)

## Supported file formats
### Import:
- PNG
- BMP
- RPGMaker 2000/2003 XYZ
- Atrophy Engine AETEX (tga/dds)
- Wii TPL*
- DDS* (DXT1,DXT2/3)
- Cave Story engine PBM
- NES (dumps CHR-ROM)
- all other formats supported by SDL2_Image

\* - not all subformats are currently implemented   

### Export:
- PNG
- OpenRaster ORA
- RPGMaker 2000/2003 XYZ
- BMP
- Cave Story engine PBM

## Building

1. Go to `freesprite/devlibs` and extract `devlibs.7z` to the current directory (right click `devlibs.7z` and `Extract here`).
2. Go back to the repository root and run the `copy_devlibs.bat` script
3. Open `freesprite.sln` with Visual Studio and build/run like any other C++ program

## Licenses

The license for voidsprite is currently to be determined.

Licenses for other libraries and fonts used in voidsprite can be found in the `OPEN_SOURCE_LICENSES` directory