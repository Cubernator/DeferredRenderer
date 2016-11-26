#! python3

# find_sources.py
# --------------------------------
# Convenience script.
# Updates cmake/source_files.cmake according to filters specified below.
# Run this script whenever you have added files to the project.

filters = [
#   | directory    | cmake variable    | regex pattern | filter name (appears in IDEs)
# --------------------------------------------------------------------------------------------
    ("include",     "HEADER_FILES",     "^.*\.h(pp)?$", ""              ),
    ("src",         "SOURCE_FILES",     "^.*\.c(pp)?$", ""              ),
    ("cmake",       "CMAKE_FILES",      ".*",           "CMake Files"   ),
    ("scripts",     "SCRIPT_FILES",     ".*",           "Script Files"  ),
    ("content",     "CONTENT_FILES",    ".*",           "Content Files" )
]

# Which file to write into (relative to project root)
filename = "cmake/source_files.cmake"

import os
import re

def add_filter(cmake_file, dir, varname, pattern, groupname):
    found_any = False
    r = re.compile(pattern)

    cmake_file.write("set(%s\n" % varname)

    for root, subdirs, files in os.walk(dir):
        for file in filter(r.match, [os.path.join(root, f) for f in files]):
            cmake_file.write("\t%s\n" % file.replace('\\', '/'))
            found_any = True

    cmake_file.write(")\n")

    if found_any and len(groupname) > 0:
        cmake_file.write("source_group(\"%s\" FILES ${%s})\n" % (groupname, varname))

    cmake_file.write("\n")

    return found_any

def main():
    os.chdir("..")

    with open(filename, "w") as cmake_file:
        varnames = []

        for filter in filters:
            if(add_filter(cmake_file, *filter)):
                varnames.append(filter[1])

        cmake_file.write("set(ALL_SOURCE_FILES\n")

        for var in varnames:
            cmake_file.write("\t${%s}\n" % var)

        cmake_file.write(")\n")

if __name__ == '__main__':
    main()