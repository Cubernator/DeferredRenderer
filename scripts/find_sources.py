#! python3

# find_sources.py
# --------------------------------
# Convenience script.
# Updates cmake/source_files.cmake according to filters specified below.
# Run this script whenever you have added files to the project.

import os
import re

FILTERS = [
    # directory   | cmake variable    | regex pattern                 | filter name (for IDEs)
    # ----------------------------------------------------------------------------------------
    ("src",         "SOURCE_FILES",     "^.*\.[hc](pp)?$",          	""),
    ("cmake",       "CMAKE_FILES",      ".*",                       	"CMake Files"),
    ("scripts",     "SCRIPT_FILES",     ".*",                       	"Script Files"),
    ("content",     "OBJECT_FILES",	    "^.*\.(json)$",             	"Object Files"),
    ("content",     "SHADER_FILES",     "^.*\.((glsl)|(glh))$",     	"Shader Files"),
    ("content",     "IMAGE_FILES",      "^.*\.((tif)|(jpg)|(png))$",   	"Image Files"),
    ("content",     "MODEL_FILES",      "^.*\.((fbx)|(blend))$",   	    "Model Files")
]

# Which file to write into (relative to project root)
FILENAME = "cmake/source_files.cmake"


def add_filter(cmake_file, dir, varname, pattern, groupname):
    found_any = False
    rexp = re.compile(pattern)

    cmake_file.write("set(%s\n" % varname)

    for root, subdirs, files in os.walk(dir):
        for file in filter(rexp.match, [os.path.join(root, f) for f in files]):
            cmake_file.write("\t%s\n" % file.replace('\\', '/'))
            found_any = True

    cmake_file.write(")\n")

    if found_any and len(groupname) > 0:
        cmake_file.write(
            "source_group(\"%s\" FILES ${%s})\n" % (groupname, varname))

    cmake_file.write("\n")

    return found_any


def main():
    os.chdir("..")

    with open(FILENAME, "w") as cmake_file:
        varnames = []

        for fil in FILTERS:
            if add_filter(cmake_file, *fil):
                varnames.append(fil[1])


if __name__ == '__main__':
    main()
