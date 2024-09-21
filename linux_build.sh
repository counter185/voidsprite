#!/bin/sh

if [ ! -e "freesprite" ]; then echo "Not in source directory"; exit 1; fi
if [ -e '/tmp/voidsprite' ]; then rm -r '/tmp/voidsprite'; fi

no_setup=
run=
prefix="/tmp/voidsprite"
portable=false
for param in $*; do
	case $param in
		"--no-setup") no_setup=1;;
		"--run") run=1;;
		"--global") prefix="/usr/local";;
		"--portable") portable=true;;
	esac
done

set -e
if [ "$no_setup" != "1" ] || [ ! -e 'build' ]; then
	if [ -e 'build' ]; then rm -r 'build'; fi
	meson setup --prefix="$prefix" -Dportable="$portable" build
fi
meson compile -C build
meson install --skip-subprojects -C build
set +e

if [ "$run" = "1" ]; then
	"$prefix"/bin/voidsprite
	exit $?
else
	echo ""
	echo "Run the compiled binary with: $prefix/bin/voidsprite"
fi
