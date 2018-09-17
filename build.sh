#!/usr/bin/env bash

# This script builds ModBox. For more information see ./build.sh --help

if [[ "$1" == "--help" ]]; then
    echo -e "Usage: $0 [--help]"
    echo -e "Build ModBox from its source, assuming that all dependencies are already installed."
    echo -e ""
    echo -e "You can pass the following environmental variables to this script:"
    echo -e "  DEBUG=[yes/no]               - build a debug (= yes) or release (= no) version of ModBox"
    echo -e "  FORCE_REBUILD=[yes/no]       - force rebuilding all sources (= yes) or only changed ones (= no)"
    echo -e "  CC_TOOLCHAIN=[gcc/clang/custom/generic] - toolchain used to build ModBox"
    echo -e "    (if CC_TOOLCHAIN=custom)"
    echo -e "      CUSTOM_CC                - C compiler"
    echo -e "      CUSTOM_CXX               - C++ compiler"
    echo -e "      CUSTOM_LD                - Linker, usually the same as CUSTOM_CXX"
    echo -e "  USE_LTO=[yes/no]             - enable link time optimization (ignored when DEBUG=yes)"
    echo -e "  PROC_COUNT=<num>             - spawn up to <num> parallel processes, set to 1 to disable parallel build"
    echo -e "  DISABLE_BOOST_STACKTRACE=[yes/no] - disable boost::stacktrace support as it is broken on some machines"
    echo -e ""
    echo -e "Default settings are:"
    echo -e "  DEBUG=no"
    echo -e "  FORCE_REBUILD=no"
    echo -e "  CC_TOOLCHAIN=generic"
    echo -e "  LTO=yes"
    echo -e "  PROC_COUNT=4"
    echo -e "  DISABLE_BOOST_STACKTRACE=no"
    echo -e ""
    echo -e "Exact commands executed are written to build.log"
    exit 1
fi

# Executable file name
exec_name="main"

# Level of optimization, ignored when environment variable 'DEBUG' is set to 'yes'
optimization_level=2

# Optimization flags (used only when DEBUG=no)
OPT_FLAGS="-O${optimization_level} -ftree-vectorize"
if [[ "${USE_LTO}" == "yes" ]]; then
    OPT_FLAGS="${OPT_FLAGS} -flto"
fi

# Compilers - default values
CC="cc"     # C compiler
CXX="c++"   # C++ compiler
LD="c++"    # Linker

# We can change toolchain if user wants to
shopt -s nocasematch
if [[ "${CC_TOOLCHAIN}" == "gcc" ]]; then  # Force usage of GCC
    CC="gcc"
    CXX="g++"
    LD="g++"
elif [[ "${CC_TOOLCHAIN}" == "clang" ]]; then  # Force usage of Clang
    CC="clang"
    CXX="clang++"
    LD="clang++"
elif [[ "${CC_TOOLCHAIN}" == "generic" ]]; then  # Force usage of generic toolchain (/usr/bin/{cc,c++})
    CC="cc"
    CXX="c++"
    LD="c++"
elif [[ "${CC_TOOLCHAIN}" == "custom" ]]; then  # Custom toolchain
    CC="${CUSTOM_CC}"
    CXX="${CUSTOM_CXX}"
    LD="${CUSTOM_LD}"
elif [[ "${CC_TOOLCHAIN}" != "" ]]; then  # Specified but unknown toolchain
    echo "Error: unknown toolchain: '${CC_TOOLCHAIN}'" >&2
    exit 1
fi
shopt -u nocasematch

# Compiler flags

# Special for C compiler
CFLAGS="-std=gnu99"
# Special for C++ compiler
CXXFLAGS="-std=gnu++14"
# Special for linker
LDFLAGS="-z relro -z now -dlsym -rdynamic"

# Flags for C and C++ compilers
FLAGS="-Wall -Wextra -pedantic -Wno-unused-parameter -Wno-unused-result -Wno-nested-anon-types -DFORTIFY_SOURCE"

if [[ "${DISABLE_BOOST_STACKTRACE}" == "yes" ]]; then
    FLAGS+=' -DNO_BOOST_STACKTRACE'
fi

# Link path, e.g. "-L/usr/lib/mylib/"
LINK_PATH=""
# Libraries to link, e.g. "-lmylib"
LIBS="-lIrrlicht -lpthread -ldl"
# Append these to linker flags, don't change this line
LDFLAGS="${LDFLAGS} ${LINK_PATH} ${LIBS}"

# Include path, e.g. "-I/usr/include/mylib"
INCLUDE_PATH="-Iinclude -I/usr/include/irrlicht"
# Append it to C/C++ compilers flags, don't change this line
FLAGS="${FLAGS} ${INCLUDE_PATH}"

# Dealing with debug mode
if [[ "${DEBUG}" == "yes" ]]; then
    # If we are debugging, set -g flag and disable optimizations
    FLAGS="${FLAGS} -g -O0 -DDEBUG_MODE -fno-inline"
    LDFLAGS="${LDFLAGS} -O0"
else
    # Othewise, optimizations are enabled
    LDFLAGS="${LDFLAGS} ${OPT_FLAGS}"
    FLAGS="${FLAGS} ${OPT_FLAGS}"
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
        echo -e "\e[1;31mError: exit status $e\e[0m" >&2
        exit 1
    fi
}

# Pretty-print command. The first argument is action such as 'cc'
# (compile C file), 'c++' (compile C++ file), or 'link'. The last function argument
# is the last command argument, and it means that is is a source/output file,
# depending on command
function dump_command() {
    case $1 in
    cc)
        echo -ne '\e[1;34m[ CC ]  \e[0m' >&2
        ;;
    cxx)
        echo -ne '\e[1;34m[ C++ ] \e[0m' >&2
        ;;
    link)
        echo -ne '\e[1;35m[LINK]  \e[0m' >&2
        ;;
    strip)
        echo -ne '\e[1;36m[STRIP] \e[0m' >&2
        ;;
    esac

    case $1 in
    cc|cxx)
        # The last function argument, see
        # https://stackoverflow.com/questions/1853946/getting-the-last-argument-passed-to-a-shell-script
        local src_file="${!#}"
        echo "$src_file" >&2
        ;;
    link)
        local final_exec_file="${!#}"
        echo "-> $final_exec_file" >&2
        ;;
    strip)
        echo "$exec_name" >&2
        ;;
    esac

    shift
    echo "$@" >> build.log
}


# Translate some_file_name.whatever -> some_file_name.o
function object_file() {
   echo "$1" | sed 's/[.][^.]*$/.o/g'
}

# Compiles file. Calls the compiler appropriate to the file extension
function build_file() {
    # Check if the source file exists
    if ! [ -f "$1" ]; then
        echo "\e[1;31mError: file '$1' not found\e[0m" >&2
        exit 1
    fi

    if ! [ -d ".build-cache/$(dirname "$1")" ]; then
        mkdir -p ".build-cache/$(dirname "$1")"
    fi
    if ! [ -d ".build-cache/$1" ]; then
        touch ".build-cache/$1"
    fi

    local objname=$(object_file "$1")

    if (sha512sum "$1" | awk '{print $1}' | cmp - .build-cache/"$1" &>/dev/null) \
            && [ ".${FORCE_REBUILD}" != ".yes" ] \
            && [ -f "${objname}" ]; then
        return 0
    fi

    sha512sum "$1" | awk '{print $1}' > .build-cache/"$1"

    # Determine which compiler should we use
    case $1 in
    *.c)
        run_command cc "${CC}" ${CFLAGS} -c -o "${objname}" "$1" >&2 &
        ;;
    *.C|*.cpp|*.c++)
        run_command cxx "${CXX}" ${CXXFLAGS} -c -o "${objname}" "$1" >&2 &
        ;;
    *)
        echo -e "\e[1;31mError: unable to build file '$1': cannot determine the compiler\e[0m" >&2
        exit 1
        ;;
    esac
}

# Builds specified files in parallel
# $1 is process count, $2 - ... are files
function thread_pool_run() {
    proc_count="$1"
    shift
    for src in "$@"; do
        if [[ $(jobs -p | wc -l) -ge "${proc_count}" ]]; then
            if ! wait -n; then
                jobs -p | xargs kill
                wait
                exit 1
            fi
        fi
        build_file "${src}"
    done
    wait
}

# OK, begin the build process
echo -e '\e[1mBuilding...\e[0m' >&2
truncate -s 0 build.log

# Find all source files and build them. We assume that file names doesn't contain
# spaces or similar stuff (because I'm too lazy to handle them correctly).
sources="$(find src/ -regex '.*[.]\(c\|C\|cpp\|c++\)' -type f)"

# The list of object files, filled in while building
objects=''
for i in ${sources}; do
    objects="${objects} $(object_file "$i")"
done

# Then we add all files to the build queue and build them
thread_pool_run ${PROC_COUNT:-4} ${sources}

# And finally link all our object files to one big executable file
run_command link "${LD}" ${objects} ${LDFLAGS} -o "${exec_name}"

# Strip the resulting file if we don't want to debug it
if [[ "${DEBUG}" != "yes" ]]; then
    run_command strip strip --strip-all "${exec_name}"
fi
