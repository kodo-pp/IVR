#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket
import base64
import sys
import math
import time
from os import _exit as exit
import threading
import traceback

VERBOSE = True

def vlog(text):
    if VERBOSE:
        print(text)
    sys.stdout.flush()

# Netcat module taken from here: https://gist.github.com/leonjza/f35a7252babdf77c8421
# and slightly modified

class Netcat:
    """ Python 'netcat like' module """

    def __init__(self, ip, port):
        self.buff = b''
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((ip, port))

    def read(self, length=1024):
        """ Read 1024 bytes off the socket """
        tmp = self.socket.recv(length)
        while len(tmp) < length:
            received = self.socket.recv(length - len(tmp))
            if len(received) == 0:
                self.socket.close()
                raise IOError('Connection closed')
            tmp += received
        return tmp

    def read_until(self, data):
        """ Read data into the buffer until we have data """
        while not data in self.buff:
            self.buff += self.socket.recv(1024)
        pos = self.buff.find(data)
        rval = self.buff[:pos + len(data)]
        self.buff = self.buff[pos + len(data):]
        return rval

    def write(self, data):
        self.socket.send(data)

    def close(self):
        self.socket.close()

class Class:
    def __init__(self, nc, name):
        self.name = name
        self.nc = nc

    def instantiate(self):
        return Object(self, self.nc, self.nc.instantiate_class(self.name))

    def get_method(self, method):
        return self.nc.invoke('core.class.getMethod', [self.name, method], 'ss', 'sss')

class Object:
    def __init__(self, cls, nc, handle):
        self.cls = cls
        self.handle = handle
        self.nc = nc

    def get_method(self, method):
        return self.cls.get_method(method)

    def call_method(self, method, ls):
        command, arg_types, ret_types = self.get_method(method)
        return self.nc.invoke(command, [self.handle] + ls, arg_types, ret_types)

class Modcat(Netcat):
    def __init__(self, ip, port):
        Netcat.__init__(self, ip, port)
        self.func_provider_names = {}
        self.func_providers = {}

    def recv_header(self):
        vlog('Reading host header')
        header = self.read(8).decode()
        if header != 'ModBox/M':
            raise Exception('Invalid server header')

    def send_header(self):
        vlog('Sending module header')
        self.write(b'ModBox/m')

    def recv_reverse_header(self):
        vlog('Reading reverse host header')
        header = self.read(8).decode()
        if header != 'ModBox/R':
            raise Exception('Invalid server header')

    def send_reverse_header(self):
        vlog('Sending reverse module header')
        self.write(b'ModBox/r')

    def read_str(self):
        vlog('{}: Reading string...'.format(id(self)))
        s = self.read_until(b'\x00')[:-1].decode()
        vlog('{}: Reading string: "{}"'.format(id(self), s))
        return s

    def write_str(self, s):
        if s is None:
            raise Exception('Attempted to send a None value')
        vlog('{}: Writing string: "{}"'.format(id(self), s))
        s = str(s)
        s += '\x00'
        self.write(bytes(s, 'ascii'))

    def unblobify(self, blob):
        vlog('unblobify: {}'.format(repr(blob)))
        split_blob = blob.split(b'\x00')

        # Костыль № 9124721
        split_blob = split_blob[:-1]

        raw_members = dict([split_blob[i:i+2] for i in range(0, len(split_blob), 2)])
        members = {
            key.decode()[:-1]: (
                key.decode()[-1], self.type_decode(value, key.decode()[-1])
            ) for key, value in raw_members.items()
        }
        return members

    def send_arg(self, arg, tp):
        vlog('send_arg: self = {}, arg = {}, tp = {}'.format(id(self), arg, tp))
        if tp in 'iufs':
            self.write_str(arg)
        elif tp == 'b':
            self.write_str(base64.b64encode(arg.encode() if type(arg) is str else arg).decode())
        else:
            raise Exception('Unknown type: "{}"'.format(tp))

    def recv_arg(self, tp):
        if tp in 'iu':
            return int(self.read_str())
        elif tp == 'f':
            return float(self.read_str())
        elif tp == 's':
            return self.read_str()
        elif tp == 'b':
            return base64.b64decode(self.read_str()).decode()
        else:
            raise Exception('Unknown type: "{}"'.format(tp))

    def blobify(self, values):
        blob = b''
        for name, (type, value) in values.items():
            blob += bytes(name, 'utf-8') + bytes(type, 'utf-8') + b'\x00'
            blob += self.type_encode(value, type) + b'\x00'
        vlog('blobify: {}'.format(repr(blob)))
        return blob

    def invoke(self, func, ls, args, ret):
        vlog('Invoking {}({})...'.format(func, ', '.join(map(str, ls))))
        self.write_str(func)
        for arg, tp in zip(ls, args):
            self.send_arg(arg, tp)
        exit_code = int(self.read_str())
        if exit_code == 0:
            ret_ls = [self.recv_arg(tp) for tp in ret]
            vlog('... = {}'.format(ret_ls))
            return ret_ls
        else:
            error = self.read_str()
            vlog('... error: {}'.format(error))
            raise Exception(error)

    def register_func_provider(self, storage, func, name, args, ret):
        vlog('Registering FuncProvider: "{}" ({}) -> {}'.format(name, args, ret))
        self.invoke('core.funcProvider.register', [name, args, ret], 'sss', '')
        storage.func_providers[name] = func, args, ret

    def maybe_register_func_provider(self, *args):
        try:
            self.register_func_provider(*args)
        except Exception as e:
            vlog('Skipped: ' + str(e))

    def serve_func(self):
        try:
            vlog('Serving')
            while True:
                # Wait for a request
                name = self.read_str()
                if name == '_exit':
                    # exit
                    return
                try:
                    # Parse the request
                    func, arg_types, ret_types = self.func_providers[name]

                    # Receive function arguments
                    args = [self.recv_arg(type) for type in arg_types]

                    vlog("calling func")
                    # Call the function
                    ret = func(*args)
                    vlog("called")
                except BaseException as e:
                    # Something has gone wrong, exit code is not 0
                    print('Exception at module function:')
                    traceback.print_exc()
                    self.write_str(1)
                    continue

                # Exit code is 0
                self.write_str(0)
                for type, val in zip(ret_types, ret):
                    self.send_arg(val, type)
        except BaseException as e:
            print('Exception occured at serve_func: ' + str(e))
            return

    def spawn_serving_thread(self):
        self.serving_thread = threading.Thread(target=self.serve_func)
        self.serving_thread.start()

    def join_serving_thread(self):
        self.serving_thread.join()

    def get_class(self, classname):
        return Class(self, classname)

    def add_class(self, classname, members, methods, parent=''):
        vlog('Adding class "{}"'.format(classname))
        members_string = ':'.join(map(lambda pair: ''.join(pair), members))
        methods_string = ':'.join(map(lambda tup: ','.join(tup), methods))
        self.invoke('core.class.add', [parent, classname, members_string, methods_string], 'ssss', '')
        return Class(self, classname)

    def instantiate_class(self, classname):
        vlog('Instantiating class {}'.format(classname))
        return self.invoke('core.class.instantiate', [classname], 's', 'u')[0]

def cube_ai(nc, handle, blob):
    return [b'', 'jump 10']

def main():
    try:
        nc = Modcat('localhost', 44145)
        rnc = Modcat('localhost', 54144)
    except ConnectionRefusedError as e:
        print('Unable to connect to host: {}'.format(e))
        exit(1)
    nc.recv_header()
    nc.send_header()
    module_name = 'Python test module'
    nc.write_str(module_name)

    rnc.recv_reverse_header()
    rnc.send_reverse_header()
    rnc.write_str(module_name)

    nc.maybe_register_func_provider(rnc, cube_ai.__get__(rnc), 'game.enemy.Cube.ai', 'ub', 'bs')

    rnc.spawn_serving_thread()

    [model_handle] = nc.invoke('graphics.drawable.createCube', [], '', 'u')
    nc.invoke('graphics.drawable.enablePhysics', [model_handle, 10, 10, 10], 'ufff', '')
    try:
        CubeEnemy = nc.add_class('game.enemy.Cube', [], [('ai', 'game.enemy.Cube.ai', 'ub', 'bs')], 'game.Enemy')
    except BaseException:
        CubeEnemy = nc.get_class('game.enemy.Cube')
    cube = CubeEnemy.instantiate()
    nc.invoke('core.class.instance.set', [cube.handle, 'model', model_handle], 'uss', '')
    nc.invoke('core.eachTickWithHandle', ['enemy.processAi', cube.handle], 'su', '')

    while True:
        time.sleep(1)

    [status] = nc.invoke('_exit', [], '', 's')

    if status == 'exited':
        print('Exited normally')
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
