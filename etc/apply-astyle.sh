#!/usr/bin/env bash
# 4-spaces indent, Java braces style (see below)
# int functionName() {
#     doSomething();
# }

astyle -s4 --style=java -rn '*.cpp'
