#!/usr/bin/env bash

# Executable file name
exec_name="main"

# Level of optimization, ignored when environment variable 'DEBUG' is set to 'yes'
optimization_level=2

# Compilers
CC="cc"     # C compiler
CXX="c++"   # C++ compiler
LD="c++"    # Linker

# Compiler flags

# Special for C compiler
CFLAGS="-std=gnu99"
# Special for C++ compiler
CXXFLAGS="-std=gnu++11"
# Special for linker
LDFLAGS=""

# Flags for C and C++ compilers
FLAGS="-Wall -Wextra -pedantic -Wno-unused-parameter -Wno-unused-result -Wno-nested-anon-types -DFORTIFY_SOURCE"

# Link path, e.g. "-L/usr/lib/mylib/"
LINK_PATH=""
# Libraries to link, e.g. "-lmylib"
LIBS="-lIrrlicht -lpthread"
# Append these to linker flags, don't change this line
LDFLAGS="${LDFLAGS} ${LINK_PATH} ${LIBS}"

# Include path, e.g. "-I/usr/include/mylib"
INCLUDE_PATH="-Iinclude -I/usr/include/irrlicht"
# Append it to C/C++ compilers flags, don't change this line
FLAGS="${FLAGS} ${INCLUDE_PATH}"

# Dealing with debug mode
if [ ".${DEBUG}" == '.yes' ]; then
    # If we are debugging, set -g flag and disable optimizations
    FLAGS="${FLAGS} -g -O0"
    LDFLAGS="${LDFLAGS} -O0"
else
    # Othewise, optimizations are set to ${optimization_level}
    LDFLAGS="${LDFLAGS} -O${optimization_level}"
    FLAGS="${FLAGS} -O${optimization_level}"
fi

# Project version
project_version="$(cat version.txt)"
FLAGS="${FLAGS} -D_PROJECT_VERSION=\"$project_version\""

# Append ${FLAGS} to CFLAGS and CXXFLAGS, don't change these lines
# All changes of FLAGS variable below these lines will be ignored
CFLAGS="${CFLAGS} ${FLAGS}"
CXXFLAGS="${CXXFLAGS} ${FLAGS}"

# Run command. The first argument is action suzh as 'cc' (compile C file),
# 'c++' (compile C++ file), or 'link'
function run_command() {
    # Pretty-print command
    dump_command "$@"
    # Then run this command
    shift
    "$@"
    # Get its exit status
    e="$?"
    # And if it is not 0 (EXIT_SUCCESS), then stop build process
    if [ "$e" -ne 0 ]; then
        echo -e "\e[1;31mError: exit status $e\e[0m"
        kill $$
    fi
}

# Pretty-print command. # Run command. The first argument is action suzh as 'cc'
# (compile C file), 'c++' (compile C++ file), or 'link'. The last function argument
# is the last command argument, and it means that is is a source/output file,
# depending on command
function dump_command() {
    case $1 in
    cc)
        echo -ne '\e[1;34m[ CC ]  \e[0m'
        ;;
    cxx)
        echo -ne '\e[1;34m[ C++ ] \e[0m'
        ;;
    link)
        echo -ne '\e[1;35m[LINK]  \e[0m'
        ;;
    strip)
        echo -ne '\e[1;36m[STRIP] \e[0m'
        ;;
    esac

    case $1 in
    cc|cxx)
        # The last function argument, see https://stackoverflow.com/questions/1853946/getting-the-last-argument-passed-to-a-shell-script
        local src_file="${!#}"
        echo "$src_file"
        ;;
    link)
        local final_exec_file="${!#}"
        echo "-> $final_exec_file"
        ;;
    strip)
        echo "$exec_name"
        ;;
    esac

    shift
    echo "$@" >> build.log
}

# Compiles file. Calls the compiler appropriate to the file extension
function build_file() {
    # Check if the source file exists
    if ! [ -f "$1" ]; then
        echo "\e[1;31mError: file '$1' not found\e[0m" >&2
        kill $$
    fi

    if ! [ -d ".build-cache/$(dirname "$1")" ]; then
        mkdir -p ".build-cache/$(dirname "$1")"
    fi
    if ! [ -d ".build-cache/$1" ]; then
        touch ".build-cache/$1"
    fi

    # Translate some_file_name.whatever -> some_file_name.o
    objname="$(echo "$i" | sed 's/[.][^.]*$/.o/g')"

    # It doesn't get printed but is appended to the 'objects' variable, see
    # this function invokation below
    echo "${objname}"

    if (sha512sum "$1" | awk '{print $1}' | cmp - .build-cache/"$1" &>/dev/null) \
            && [ ".${FORCE_REBUILD}" != ".yes" ] \
            && [ -f "${objname}" ]; then
        return 0
    fi

    sha512sum "$1" | awk '{print $1}' > .build-cache/"$1"

    # Determine which compiler should we use
    case $1 in
    *.c)
        run_command cc "${CC}" ${CFLAGS} -c -o "${objname}" "$1" >&2
        ;;
    *.C|*.cpp|*.c++)
        run_command cxx "${CXX}" ${CXXFLAGS} -c -o "${objname}" "$1" >&2
        ;;
    *)
        echo -e "\e[1;31mError: unable to build file '$1': cannot determine the compiler\e[0m" >&2
        kill $$
        ;;
    esac
}

# OK, begin the build process
echo -e '\e[1mBuilding...\e[0m'
truncate -s 0 build.log

# The list of object files, filled in while building
objects=''

# Find all source files and build them. We assume that file names doesn't contain
# spaces or similar stuff (because I'm too lazy to handle them correctly).
for i in $(find src/ -regex '.*[.]\(c\|C\|cpp\|c++\)' -type f); do
    objects="${objects} $(build_file "$i")"
done

# And finally link all our object files to one big executable file
run_command link "${LD}" ${objects} ${LDFLAGS} -o "${exec_name}"

# Strip the resulting file if we don't want to debug it
if [ ".$DEBUG" != ".yes" ]; then
    run_command strip strip --strip-all "${exec_name}"
fi
