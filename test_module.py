#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket
import base64
import math
import time
from os import _exit as exit
import threading
import traceback

VERBOSE = True

def vlog(text):
    if VERBOSE:
        print(text)

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
        self.command_handles = {}
        self.reserved_handle = 0xFFFFFFFF4E5E47ED
        self.func_provider_names = {}
        self.func_providers = {}

    def read_int(self, size=4, signed=False):
        tmp = self.read(size)
        return int.from_bytes(tmp, 'little', signed=signed)

    def write_int(self, n, size=4, signed=False):
        self.write(n.to_bytes(size, 'little', signed=signed))

    def read_float(self, bits):
        assert bits in [32, 64]
        return float(self.read_str())

    def write_float(self, v, bits):
        assert bits in [32, 64]
        self.write_str(str(v))

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
        return self.read_until(b'\x00')[:-1].decode()

    def write_str(self, s):
        s += '\x00'
        self.write(bytes(s, 'ascii'))

    def read_blob(self):
        size = self.read_int(8, signed=False)
        return self.read(size)

    def write_blob(self, blob):
        self.write_int(len(blob), 8, signed=False)
        self.write(blob)

    def send_arg(self, arg, tp):
        if tp == 'b':
            self.write_int(arg, 1, signed=True)
        elif tp == 'h':
            self.write_int(arg, 2, signed=True)
        elif tp == 'i':
            self.write_int(arg, 4, signed=True)
        elif tp == 'l':
            self.write_int(arg, 8, signed=True)
        elif tp == 'B':
            self.write_int(arg, 8, signed=False)
        elif tp == 'H':
            self.write_int(arg, 2, signed=False)
        elif tp == 'I':
            self.write_int(arg, 4, signed=False)
        elif tp == 'L':
            self.write_int(arg, 8, signed=False)
        elif tp == 'f':
            self.write_float(arg, 32)
        elif tp == 'F':
            self.write_float(arg, 64)
        elif tp == 's':
            self.write_str(arg)
        elif tp == 'w':
            raise Exception('Wide string is unimplemented')
        elif tp == 'o':
            self.write_blob(arg)
        else:
            raise Exception('Unknown type: "{}"'.format(tp))

    def recv_arg(self, tp):
        if tp == 'b':
            return self.read_int(1, signed=True)
        elif tp == 'h':
            return self.read_int(2, signed=True)
        elif tp == 'i':
            return self.read_int(4, signed=True)
        elif tp == 'l':
            return self.read_int(8, signed=True)
        elif tp == 'B':
            return self.read_int(8, signed=False)
        elif tp == 'H':
            return self.read_int(2, signed=False)
        elif tp == 'I':
            return self.read_int(4, signed=False)
        elif tp == 'L':
            return self.read_int(8, signed=False)
        elif tp == 'f':
            return self.read_float(32)
        elif tp == 'F':
            return self.read_float(64)
        elif tp == 's':
            return self.read_str()
        elif tp == 'w':
            raise Exception('Wide string is unimplemented')
        elif tp == 'o':
            return self.read_blob()
        else:
            raise Exception('Unknown type: "{}"'.format(tp))

    def type_decode(self, enc, type):
        if type in 'bBhHiIlL':
            return int(enc.decode())
        elif type in 'fF':
            return float(enc.decode())
        elif type == 's':
            return enc.decode()
        elif type == 'o':
            raise Exception('BLOB is unimplemented')
        elif type == 'w':
            raise Exception('Wide string is unimplemented')
        else:
            raise Exception('Unknown type: "{}"'.format(type))

    def type_encode(self, value, type):
        if type in 'bBhHiIlLfFs':
            return bytes(str(value), 'utf-8')
        elif type == 'o':
            raise Exception('BLOB is unimplemented')
        elif type == 'w':
            raise Exception('Wide string is unimplemented')
        else:
            raise Exception('Unknown type: "{}"'.format(type))


    def unblobify(self, blob):
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

    def blobify(self, values):
        blob = b''
        for name, (type, value) in values.items():
            blob += bytes(name, 'utf-8') + bytes(type, 'utf-8') + b'\x00'
            blob += self.type_encode(value, type) + b'\x00'
        return blob

    def invoke(self, func, ls, args, ret):
        vlog('Invoking {}({})...'.format(func, ', '.join(map(str, ls))))
        if func not in self.command_handles:
            self.write_int(0, size=8, signed=False)
            self.write_str(func)
            handle = self.read_int(size=8, signed=False)
            if handle == 0:
                vlog('... no such function')
                raise Exception('No such function: "{}"'.format(func))
            self.command_handles[func] = handle

        self.write_int(self.command_handles[func], size=8, signed=False)
        for arg, tp in zip(ls, args):
            self.send_arg(arg, tp)
        exit_code = self.read_int(1, signed=False)
        if exit_code == 0:
            ret_ls = [self.recv_arg(tp) for tp in ret]
            vlog('... = {}'.format(ret_ls))
            return ret_ls
        else:
            error = self.read_str()
            vlog('... error: {}'.format(error))
            raise Exception(error)

    def invoke_special(self, func, ls, args, ret):
        vlog('Invoking special function {}({})...'.format(func, ', '.join(map(str, ls))))
        self.write_int(self.reserved_handle, size=8, signed=False)
        self.write_str(func)
        for arg, tp in zip(ls, args):
            self.send_arg(arg, tp)
        ret_ls = [self.recv_arg(tp) for tp in ret]
        vlog('... = {}'.format(ret_ls))
        return ret_ls

    def register_func_provider(self, storage, func, name, args, ret):
        vlog('Registering FuncProvider: "{}" ({}) -> {}'.format(name, args, ret))
        [handle] = self.invoke('core.funcProvider.register', [name, args, ret], 'sss', 'L')
        storage.func_provider_names[handle] = name
        storage.func_providers[name] = func, args, ret

    def serve_func(self):
        try:
            vlog('Serving')
            while True:
                # Wait for a request
                handle = self.read_int(8, signed=False)
                print('Handle = {}'.format(handle))
                if handle == self.reserved_handle:
                    # exit
                    return
                try:
                    # Parse the request
                    name = self.func_provider_names[handle]
                    func, arg_types, ret_types = self.func_providers[name]

                    # Receive function arguments
                    args = [self.recv_arg(type) for type in arg_types]

                    # Call the function
                    ret = func(*args)
                except BaseException as e:
                    # Something has gone wrong, exit code is not 0
                    print('Exception at module function:')
                    traceback.print_exc()
                    self.write_int(1, size=1, signed=False)
                    continue

                # Exit code is 0
                self.write_int(0, size=1, signed=False)
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

    def class_handle(self, classname):
        return self.invoke('core.class.getHandle', [classname], 's', 'L')[0]

    def get_class(self, classname):
        return Class(self, self.class_handle(classname))

    def add_class(self, classname, members, methods, parent=''):
        vlog('Adding class "{}"'.format(classname))
        members_string = ':'.join(map(lambda pair: ''.join(pair), members))
        methods_string = ':'.join(map(lambda tup: ','.join(tup), methods))
        self.invoke('core.class.add', [parent, classname, members_string, methods_string], 'ssss', '')
        return Class(self, classname)

    def instantiate_class(self, classname):
        vlog('Instantiating class {}'.format(classname))
        return self.invoke('core.class.instantiate', [classname], 's', 'L')[0]

def baz(nc, handle, blob, number):
    print('baz: blob: ' + base64.b64encode(blob).decode())
    print(repr(nc.unblobify(blob)))
    retblob = nc.blobify({'foo': ('i', 166)})
    print('baz: retblob: ' + base64.b64encode(retblob).decode())
    return [retblob, number + 5]

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

    nc.register_func_provider(rnc, baz.__get__(rnc), 'test.baz', 'Loi', 'oi')

    rnc.spawn_serving_thread()

    TestClass = nc.add_class('test.TestClass', [('foo', 'i'), ('bar', 'i')], [('baz', 'test.baz', 'Loi', 'oi')], '')
    test_class = TestClass.instantiate()

    nc.invoke('core.class.instance.setInt32', [test_class.handle, 'foo', 5], 'Lsi', '')
    nc.invoke('core.class.instance.setInt32', [test_class.handle, 'bar', 3], 'Lsi', '')
    nc.invoke('core.class.instance.getInt32', [test_class.handle, 'foo'], 'Ls', 'i')
    nc.invoke('core.class.instance.getInt32', [test_class.handle, 'bar'], 'Ls', 'i')
    test_class.call_method('baz', [77])
    nc.invoke('core.class.instance.getInt32', [test_class.handle, 'foo'], 'Ls', 'i')
    nc.invoke('core.class.instance.getInt32', [test_class.handle, 'bar'], 'Ls', 'i')

    [status] = nc.invoke_special('exit', [], '', 's')

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
