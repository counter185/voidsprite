#!/usr/bin/python3

import xml.etree.ElementTree as ET
import os.path
from typing import List

# --- CONFIGURATION VARIABLES START ---

MESON_PROJECT = "voidsprite"
# we ignore vendored library sources since we build them with subprojects
# (or don't build them at all, if we use system libraries or meson wraps)
IGNORED_SUBDIRS = ["libpng", "pugixml", "liblcf", "libtga"]

# --- CONFIGURATION VARIABLES END ---

if not os.path.exists("freesprite.vcxproj"):
    print(f"Couldn't find file freesprite.vcxproj, are you in the source folder?")
    quit(1)

root = ET.parse("freesprite.vcxproj").getroot()
namespace = {"": "http://schemas.microsoft.com/developer/msbuild/2003"}

sources = []

for target in root.iterfind("ItemGroup/ClCompile", namespace):
    source_filename = os.path.join(*(target.attrib["Include"].replace("\\", "/").split("/")))
    sources.append(source_filename)

meson_sources = {MESON_PROJECT: []}

for source in sources:
    if source.split("/")[0] in IGNORED_SUBDIRS:
        continue
    else:
        meson_sources[MESON_PROJECT].append(source)

print('\n'.join(meson_sources[MESON_PROJECT]))
