#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import subprocess as sp
from sys import argv

cxxflags = [
    '-D_PROJECT_VERSION="1.0"',
    '-Iinclude',
    '-I/usr/include',
    '-I/usr/include/irrlicht',
]

sources = [b.decode() for b in sp.check_output(['find', 'src/', '-name', '*.cpp']).split(b'\n')]

checks = [
#    'cppcoreguidelines-*',
    'readability-*',
    'portability-*',
#    'modernize-*',
    'performance-*',
    '-readability-else-after-return',
]

checks_flag = '--checks={}'.format(','.join(checks))
sp.call(['/usr/bin/env', 'clang-tidy', checks_flag] + argv[1:] + sources + ['--'] + cxxflags)
