#!/usr/bin/env bash

exec_name="main"

if ! [ -f "${exec_name}" ] || [ ".$1" == ".-f" ]; then
    ./build.sh || exit 1
fi

"./${exec_name}"
