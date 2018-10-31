#!/usr/bin/env bash

if [[ -z "$1" || "$1" == "--help" || "$1" == "-help" || "$1" == "-h" ]]; then
    echo "Usage: $0 <irrlicht_source_directory>" >&2
    echo "Generates string->enum map for Irrlicht enums" >&2
    exit 1
fi


wd="${PWD}"
cd "$1"
gen_file="${wd}/src/graphics/enums.cpp"
blacklist_file="${wd}/etc/enum-blacklist.txt"
function genfile_prepare() {
    echo "// This file is auto-generated, do not edit it by hand" > "${gen_file}"
    echo "#include <modbox/graphics/enums.hpp>" >> "${gen_file}"
    echo "#include <string>" >> "${gen_file}"
    echo "#include <stdexcept>" >> "${gen_file}"
    echo "#include <irrlicht_wrapper.hpp>" >> "${gen_file}"
    echo "int stringToIrrlichtEnum(const std::string& s) {" >> "${gen_file}"
    echo "    using namespace irr;" >> "${gen_file}"
    echo "    using namespace core;" >> "${gen_file}"
    echo "    using namespace scene;" >> "${gen_file}"
    echo "    using namespace video;" >> "${gen_file}"
    echo "    using namespace gui;" >> "${gen_file}"
    echo "    using namespace io;" >> "${gen_file}"
    echo "    using namespace quake3;" >> "${gen_file}"
}
function genfile_write() {
    echo "$@" #>> "${gen_file}"
}
function genfile_finalize() {
    echo "    throw std::runtime_error(\"Unknown Irrlicht enum value: '\" + s + \"'\");" >> "${gen_file}"
    echo "}" >> "${gen_file}"
    echo "Successfully generated ${gen_file}" >&2
}

echo "Looking for enums..." >&2
file_list=($(find . -name '*.h' -type f))
enum_list=($(grep -Ehoe 'enum E[A-Z0-9_]+' "${file_list[@]}" | sed 's/^enum //g'))
enums_count="${#enum_list[@]}"
echo "Found ${enums_count} enums" >&2

genfile_prepare
{
    i=0
    for enum in "${enum_list[@]}"; do
        (( ++i ))
        echo "($i / ${enums_count}) Processing enum '${enum}'..." >&2
        # enum_prefix=$(for c in $(echo "${enum}" | sed 's/_/ /g'); do printf "%c" "$c"; done)
        list=($(
            find . -name '*.h' -type f \
            | xargs cat \
            | grep -Ezoe 'enum '"${enum}"'\W*\{[^}]*\};' \
            | tr -d '\0' \
            | grep -Ewoe "E[^_]+_[A-Z0-9_]+"
        ))
        for j in "${list[@]}"; do
            genfile_write "    if (s == \"$j\") { return static_cast<int>($j); }"
        done
    done
} | grep -Ee '\(E[A-Z0-9_]+\)' | grep -Fwvf "${blacklist_file}" | sort | uniq >> "${gen_file}"
genfile_finalize

echo "Formatting generated file with clang-format..."
cd "${wd}"
clang-format -i "${gen_file}" || true
