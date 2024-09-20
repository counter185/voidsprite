#!/bin/sh

if [ ! -e "freesprite" ]; then echo "Not in source directory"; exit 1; fi
if [ -e '/tmp/voidsprite' ]; then rm -r '/tmp/voidsprite'; fi

no_setup=
run=
for param in $*; do
	case $param in
		"--no-setup") no_setup=1;;
		"--run") run=1;;
	esac
done

set -e
if [ "$no_setup" != "1" ]; then
	meson setup --wipe --prefix=/tmp/voidsprite build
fi
meson compile -C build
meson install --skip-subprojects -C build
set +e

if [ "$run" = "1" ]; then
	/tmp/voidsprite/bin/voidsprite
	exit $?
else
	echo "Run the compiled binary with: /tmp/voidsprite/bin/voidsprite"
fi
