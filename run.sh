#!/usr/bin/env bash

exec_name="main"

if ! [ -f "${exec_name}" ] || [ ".$1" == ".-f" ]; then
    ./build.sh
fi

echo -e "\e[1;32m[ RUN ] \e[0m${exec_name}"
"./${exec_name}"
