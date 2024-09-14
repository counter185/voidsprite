![voidsprite logo](freesprite/assets/mainlogo.png)

# voidsprite

Free C++ sprite editor

![Preview image](img_preview_1.png)

## Supported file formats
### Import:
- PNG
- BMP
- OpenRaster ORA
- RPGMaker 2000/2003 XYZ
- Atrophy Engine AETEX (tga/dds)
- Wii TPL*
- DDS* (DXT1, DXT2/3, DXT4/5)
- Cave Story engine PBM
- VTF* (BGRA, BGR, DXT1, DXT2/3, DXT4/5)
- NES (dumps CHR-ROM)
- Mario Paint save file (SRM)
- all other formats supported by SDL2_Image

\* - not all subformats are currently implemented   

### Export:
- PNG
- OpenRaster ORA
- RPGMaker 2000/2003 XYZ
- BMP
- Cave Story engine PBM
- C header (as `uint8_t` array)
- Python file (as [R,G,B,A] `np.array`)
- HTML document (inline Base64 image)

## Building

1. Go to `freesprite/devlibs` and extract `devlibs.7z` to the current directory (right click `devlibs.7z` and `Extract here`).
2. Go back to the repository root and run the `copy_devlibs.bat` script
3. Open `freesprite.sln` with Visual Studio and build/run like any other C++ program

## Licenses

voidsprite is licensed under GPLv2.

Licenses for other libraries and fonts used in voidsprite can be found in the `OPEN_SOURCE_LICENSES` directory
