#!/bin/sh
cd freesprite
find . -name "*.cpp" -exec echo "#include \"{}\"" >> "vsp_unity.cpp" \;
