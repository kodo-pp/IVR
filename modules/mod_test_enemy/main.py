#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import math
import threading
import traceback
import pymodbox
import time
from os import _exit as exit

def test_enemy_creation(module, handle):
    return []

def test_enemy_ai(module, handle, x, y, z, player_x, player_y, player_z):
    return ['jump 10']

def main():
    module = pymodbox.Module('test_enemy')
    module.exit_on_callback_errors = True
    module.register_func_provider(test_enemy_creation, 'testEnemy.creation', 'u', '')
    module.register_func_provider(test_enemy_ai, 'testEnemy.ai', 'uffffff', 's')
    module.ready()
    module.invoke('enemy.addKind', ['testEnemy', 'testEnemy.creation', 'testEnemy.ai'], 'sss', '')

    [drawable_handle] = module.invoke('graphics.drawable.createCube', [], '', 'u')
    module.invoke('graphics.drawable.enablePhysics', [drawable_handle, 1, 1, 1], 'ufff', '')
    [enemy_handle] = module.invoke('enemy.add', ['testEnemy', drawable_handle], 'su', 'u')

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
