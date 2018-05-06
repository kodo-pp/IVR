#!/usr/bin/env bash

exec_name="main"

echo -e '\e[1;33m[ RM ]  \e[0mRemoving object files'
find src/ -name '*.o' -delete
echo -e "\e[1;33m[ RM ]  \e[0mRemoving ${exec_name}"
rm -f "${exec_name}"
