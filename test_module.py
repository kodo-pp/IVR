#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import math
import threading
import traceback
import pymodbox
import time
from os import _exit as exit

def cube_ai(module, handle, blob):
    return [b'', 'jump 10']

def main():
    module = pymodbox.Module('Test module')
    module.register_func_provider(cube_ai, 'game.enemy.Cube.ai', 'ub', 'bs')

    [model_handle] = module.invoke('graphics.drawable.createCube', [], '', 'u')
    module.invoke('graphics.drawable.enablePhysics', [model_handle, 10, 10, 10], 'ufff', '')
    try:
        CubeEnemy = module.nc.add_class('game.enemy.Cube', [], [('ai', 'game.enemy.Cube.ai', 'ub', 'bs')], 'game.Enemy')
    except BaseException:
        CubeEnemy = module.nc.get_class('game.enemy.Cube')
    cube = CubeEnemy.instantiate()
    module.invoke('core.class.instance.set', [cube.handle, 'model', model_handle], 'uss', '')
    module.invoke('core.eachTickWithHandle', ['enemy.processAi', cube.handle], 'su', '')

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
