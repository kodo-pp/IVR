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

counter = 0

last_player_attack = 0.0
attack_max_each = 1.0

def test_enemy_ai(module, handle, x, y, z, player_x, player_y, player_z):
    global counter
    counter += 1

    global last_player_attack, attack_max_each
    if counter % 4 == 0 and time.time() - last_player_attack >= attack_max_each:
        last_player_attack = time.time()
        return ['attackPlayer 100 0.1']
    elif counter % 4 == 1:
        return ['jump 10']
    elif counter % 4 == 2:
        return ['setSpeed 50']
    else:
        return ['lookAt {} {} {}'.format(player_x, player_y, player_z)]

def main():
    module = pymodbox.Module('test_enemy')
    module.exit_on_callback_errors = True
    module.register_func_provider(test_enemy_creation, 'testEnemy.creation', 'u', '')
    module.register_func_provider(test_enemy_ai, 'testEnemy.ai', 'uffffff', 's')
    module.ready()
    module.invoke('enemy.addKind', ['testEnemy', 'testEnemy.creation', 'testEnemy.ai', 3.0], 'sssf', '')
    [drawable_handle] = module.invoke('graphics.drawable.createCube', [], '', 'u')
    module.invoke('graphics.drawable.setScale', [drawable_handle, 5, 5, 5], 'ufff', '')
    module.invoke('graphics.drawable.enablePhysics', [drawable_handle, 5, 5, 5], 'ufff', '')
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
