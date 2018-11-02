#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import math
import threading
import traceback
import pymodbox
import time
import json
from os import _exit as exit
from base64 import b64encode, b64decode

current_inventory_cell = 0

class Inventory:
    def __init__(self, size, max_stack):
        self.items = [[0, 'nil']] * size
        self.size = size
        self.max_stack = max_stack
        self.rectangles = None
        self.rect = [.05, .9, .3, .95]
        self.text_handle = None

    def __getitem__(self, index):
        return self.items[index]

    def dump(self):
        return json.dumps(self.items)

    def load(self, enc):
        self.items = json.loads(enc)
        self.size = len(self.items)

    def add(self, item, count):
        candidates = []
        for index, (stored_count, stored_item) in enumerate(self.items):
            if stored_item == item and stored_count < self.max_stack:
                candidates.append(index)
        candidates += list(range(self.size))
        for index in candidates:
            if count == 0:
                break
            stored_count, stored_item = self.items[index]
            if stored_item == item or stored_count == 0 or stored_item == 'nil':
                delta = min(count, self.max_stack - stored_count)
                self.items[index] = stored_count + delta, item
                count -= delta
        global current_inventory_cell
        self.draw(current_inventory_cell)
        return count

    def remove(self, item, count):
        global current_inventory_cell
        removed = 0
        candidates = [current_inventory_cell]
        # module.log("remove: self.items = {}".format(self.items))
        for index, (stored_count, stored_item) in enumerate(self.items):
            # module.log("remove: item = {}, sitem = {}, scount = {}".format(item, stored_item, stored_count))
            if stored_item == item and stored_count > 0:
                candidates.append(index)
        # module.log("remove: candidates = {}".format(candidates))
        for index in candidates:
            stored_count, stored_item = self.items[index]
            if stored_item != item:
                continue
            delta = min(count - removed, stored_count)
            removed += delta
            self.items[index] = stored_count - delta, item if stored_count - delta > 0 else 'nil'
        self.draw(current_inventory_cell)
        return removed

    def draw(self, current):
        global module
        if self.text_handle is None:
            [self.text_handle] = module.invoke('graphics.2d.addText', [*self.rect, ''], 'ffffs', 'u')
        text = 'Inventory: ({} / {}) '.format(current + 1, self.size)
        if self.items[current][0] == 0 or self.items[current][1] == 'nil':
            text += '<empty>'
        else:
            text += '{} x {}'.format(*self.items[current])
        module.invoke('graphics.2d.modifyText', [self.text_handle, *self.rect, text], 'uffffs', '')

current_inventory = Inventory(size=10, max_stack=100)

module = None

def is_current_inventory_cell_empty(module):
    global current_inventory, current_inventory_cell
    return [1 if current_inventory[current_inventory_cell][0] == 0 else 0]

def get_current_inventory_cell_count(module):
    global current_inventory, current_inventory_cell
    return [current_inventory[current_inventory_cell][0]]

def get_current_inventory_cell_item(module):
    global current_inventory, current_inventory_cell
    return [current_inventory[current_inventory_cell][1]]

def add_items_to_inventory(module, item, count):
    global current_inventory
    return [current_inventory.add(item, count)]

def remove_items_from_inventory(module, item, count):
    global current_inventory
    return [current_inventory.remove(item, count)]

def set_current_inventory_cell(module, cellno):
    global current_inventory, current_inventory_cell
    current_inventory_cell = cellno % current_inventory.size
    current_inventory.draw(current_inventory_cell)
    return []

def shift_current_inventory_cell(module, count):
    global current_inventory, current_inventory_cell
    current_inventory_cell = (current_inventory_cell + count + current_inventory.size) % current_inventory.size
    current_inventory.draw(current_inventory_cell)
    return []

def main():
    global module
    module = pymodbox.Module('inventory')
    module.register_func_provider(is_current_inventory_cell_empty, 'inventory.isCurrentCellEmpty', '', 'i')
    module.register_func_provider(get_current_inventory_cell_count, 'inventory.getCurrentCellCount', '', 'u')
    module.register_func_provider(get_current_inventory_cell_item, 'inventory.getCurrentCellItem', '', 's')
    module.register_func_provider(set_current_inventory_cell, 'inventory.setCurrentCell', 'i', '')
    module.register_func_provider(shift_current_inventory_cell, 'inventory.shiftCurrentCell', 'i', '')
    module.register_func_provider(add_items_to_inventory, 'inventory.addItems', 'su', 'u')
    module.register_func_provider(remove_items_from_inventory, 'inventory.removeItems', 'su', 'u')

    module.ready()

    global current_inventory, current_inventory_cell
    current_inventory.draw(current_inventory_cell)
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
