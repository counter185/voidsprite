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

evalLibs () {
    otool -L $1 | grep -oE '\t.*\(' | cut -d' ' -f1 | cut -d$'\t' -f2 | grep -v ^'/usr/lib' | grep -v ^'/System/Library'
}

copyLib () {
    libname=$(echo $1 | rev | cut -d'/' -f-1 | rev)
    cp $1 voidsprite.app/Contents/MacOS/
    install_name_tool -change $1 @executable_path/$libname voidsprite.app/Contents/MacOS/voidsprite
}

for lib in $(evalLibs "target/release/bin/voidsprite")
do
    dirname=$(dirname $lib)
    for sublib in $(evalLibs $lib)
    do
        #this will give us some Permission denied errors on duplicate libs but we don't care
        realpath=${sublib/@rpath/$dirname}
        echo "- sublib" $realpath
        copyLib $realpath
    done
    echo "Copying" $lib
    copyLib $lib
done

mkdir voidsprite.app/Contents/MacOS/assets
cp -R freesprite/assets voidsprite.app/Contents/MacOS
cp freesprite/*.ttf voidsprite.app/Contents/MacOS/
cp -R OPEN_SOURCE_LICENSES voidsprite.app
mkdir target/mac_release
mv voidsprite.app target/mac_release/voidsprite.app