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
    module = pymodbox.Module('test_game_object')
    module.ready()

    module.invoke('gameObject.addKind', ['testGameObject'], 's', '')
    module.invoke('gameObject.addKind', ['anotherGameObject'], 's', '')
    [drawable_handle] = module.invoke('graphics.drawable.createCube', [], '', 'u')
    module.invoke('graphics.drawable.setScale', [drawable_handle, 7, 7, 7], 'ufff', '')
    module.invoke('graphics.drawable.enablePhysics', [drawable_handle, 7, 7, 7], 'ufff', '')
    module.invoke('graphics.drawable.enableCollisions', [drawable_handle], 'u', '')
    [object_handle] = module.invoke('gameObject.add', ['testGameObject', drawable_handle], 'su', 'u')

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
