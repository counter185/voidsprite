#!/bin/sh

unset prefix

init=$([ -f build/build.ninja ]; echo $?)
run=
_prefix="${PWD}/target/debug"
portable=false
buildtype=debug
keep=false
while [ $# -gt 0 ]; do
	case $1 in
		"--no-setup") init=0; shift;;
		"--run") run=1; shift;;
		"--global") prefix="/usr/local"; shift;;
		"--portable") portable=true; shift;;
		"--prefix") prefix="$2"; shift; shift;;
		"--release") buildtype=release; _prefix="${PWD}/target/release"; shift;;
		"--keep") keep=true; shift;;
		"--init") init=1; shift;;
	esac
done

prefix="${prefix:-${_prefix}}"

if [ ! -e "freesprite" ]; then echo "Not in source directory"; exit 1; fi
if [ "$keep" = "false" ] && [ ! "$prefix" ] && [ "$prefix" != "/usr" ]; then if [ -e "$prefix" ]; then rm -r "$prefix"; fi; fi

set -e
if [ "$init" == "1" ] || [ ! -e 'build' ]; then
	if [ -e 'build' ]; then rm -r 'build'; fi
	meson setup --prefix="$prefix" -Dportable="$portable" --buildtype="$buildtype" build
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
