#!/usr/bin/env bash

# unset prefix
# 
# init=$([ -f build/build.ninja ]; echo $?)
# run=
# _prefix="${PWD}/target/debug"
# portable=false
# buildtype=debug
# keep=false
# while [ $# -gt 0 ]; do
# 	case $1 in
# 		"--no-setup") init=0; shift;;
# 		"--run") run=1; shift;;
# 		"--global") prefix="/usr/local"; shift;;
# 		"--portable") portable=true; shift;;
# 		"--prefix") prefix="$2"; shift; shift;;
# 		"--release") buildtype=release; _prefix="${PWD}/target/release"; shift;;
# 		"--keep") keep=true; shift;;
# 		"--init") init=1; shift;;
# 	esac
# done
# 
# prefix="${prefix:-${_prefix}}"
# 
# if [ ! -e "freesprite" ]; then echo "Not in source directory"; exit 1; fi
# if [ "$keep" = "false" ] && [ ! "$prefix" ] && [ "$prefix" != "/usr" ]; then if [ -e "$prefix" ]; then rm -r "$prefix"; fi; fi
# 
# set -e
# if [ "$init" == "1" ] || [ ! -e 'build' ]; then
# 	if [ -e 'build' ]; then rm -r 'build'; fi
# 	meson setup --prefix="$prefix" -Dportable="$portable" --buildtype="$buildtype" build
# fi
# meson compile -C build
# meson install --skip-subprojects -C build
# set +e
# 
# if [ "$run" = "1" ]; then
# 	"$prefix"/bin/voidsprite
# 	exit $?
# else
# 	echo ""
# 	echo "Run the compiled binary with: $prefix/bin/voidsprite"
# fi

set -e

cd "$(dirname "$(realpath "$0")")"

prefix=".."
buildtypep=debug
buildtype=Debug
makeg="Unix Makefiles"
make=make
run=0
install=0

build_sdl3=0

if command -v ninja &> /dev/null; then
    makeg=Ninja
    make=ninja
fi

while [ $# -gt 0 ]; do
    case $1 in
        "--run") run=1; shift;;
        "--install") install=1; shift;;
        "--global") prefix="/usr/share/voidsprite"; shift;;
        "--prefix") shift; prefix="$1"; shift;;
        "--release") buildtype=Release; buildtypep=eelease; shift;;
        "--init") init=1; shift;;

        "--build-sdl3") build_sdl3=1; shift;;
    esac
done

# if [ "$build_sdl3" == "1" ] && [ ! -d lib/SDL3 ]; then
#     git clone https://github.com/libsdl-org/SDL --branch release-3.2.10 lib/SDL3 --depth 1
#     > lib/SDL3/SDL3-config.cmake <<EOF
# add_subdirectory(lib/SDL3)
# EOF
# fi

mkdir -p build
(cmake . -B build/$buildtypep \
    -G $makeg $(if [ "$build_sdl3" == "1" ]; then echo -DVOIDSPRITE_STATIC_SDL3=ON; fi) \
    -DCMAKE_BUILD_TYPE=$buildtype \
    -DVOIDSPRITE_ASSETS_PATH=${prefix}/share/voidsprite && (cd build/$buildtypep && $make)) 2>&1 | tee fuck
test ${PIPESTATUS[0]} -eq 0

rm -fr target/$buildtypep
mkdir -p target/$buildtypep/{bin,share/voidsprite}

cp build/$buildtypep/voidsprite target/$buildtypep/bin/
cp -r build/$buildtypep/assets target/$buildtypep/share/voidsprite/

if [ "$run" == "1" ]; then
    target/$buildtypep/bin/voidsprite
fi
