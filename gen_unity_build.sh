#!/bin/sh
cd freesprite
find . -name "*.cpp" -exec echo "#include \"{}\"" >> "vsp_unity.txt" \;
mv ./vsp_unity.txt ./vsp_unity.cpp