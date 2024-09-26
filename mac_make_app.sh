#!/bin/sh

mkdir voidsprite.app
mkdir voidsprite.app/Contents
mkdir voidsprite.app/Contents/MacOS
mkdir voidsprite.app/Contents/Resources
cp mac-build-resources/Info.plist voidsprite.app/Contents/Info.plist
cp target/release/bin/voidsprite voidsprite.app/Contents/MacOS/voidsprite
chmod +x voidsprite.app/Contents/MacOS/voidsprite

mkdir mac-build-resources/generated
mkdir mac-build-resources/generated/voidsprite.iconset
sips -z 32 32 freesprite/assets/icon_voidsprite32x32.png --out mac-build-resources/generated/voidsprite.iconset/icon_32x32.png
sips -z 64 64 freesprite/assets/icon_voidsprite32x32.png --out mac-build-resources/generated/voidsprite.iconset/icon_32x32@2x.png
iconutil -c icns mac-build-resources/generated/voidsprite.iconset
cp mac-build-resources/generated/voidsprite.icns voidsprite.app/Contents/Resources/voidsprite.icns

foundlibs=$(otool -L target/release/bin/voidsprite | grep -oE '\t.*\(' | cut -d' ' -f1 | cut -d$'\t' -f2 | grep -v ^'/usr/lib' | grep -v ^'/System/Library')

for lib in $foundlibs
do
echo "Copying" $lib
libname=$(echo $lib | rev | cut -d'/' -f-1 | rev)
cp $lib voidsprite.app/Contents/MacOS/
install_name_tool -change $lib @executable_path/$libname voidsprite.app/Contents/MacOS/voidsprite
done

mkdir voidsprite.app/Contents/MacOS/assets
cp -R freesprite/assets voidsprite.app/Contents/MacOS
cp freesprite/*.ttf voidsprite.app/Contents/MacOS/
cp -R OPEN_SOURCE_LICENSES voidsprite.app
mkdir target/mac_release
mv voidsprite.app target/mac_release/voidsprite.app