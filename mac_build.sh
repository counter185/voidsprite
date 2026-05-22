#!/bin/sh
#./linux_build.sh --release --portable

mkdir build
cd build
cmake .. -DRELEASE=ON
make -j4
cd ..

./mac_make_app.sh