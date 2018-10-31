#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import math
import threading
import traceback
import pymodbox
import time
from os import _exit as exit

def main():
    module = pymodbox.Module('aim')
    module.ready()
    rect = [.48, .48, .52, .52]
    color = [0, 0, 0, 100]
    module.invoke('graphics.2d.addRectangle', [*rect, *color], 'ffffiiii', 'u')

    while True:
        time.sleep(1)

    [status] = module.invoke('_exit', [], '', 's')

    if status == 'exited':
        log('Exited normally')
        exit(0)
    else:
        raise Exception('Unknown exit status: "{}"'.format(status))
if __name__ == '__main__':
    try:
        main()
        exit(0)
    except BaseException as e:
        traceback.print_exc()
        exit(1)
