#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket
import base64
import math
import time
from os import _exit as exit
import threading

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
            tmp += self.socket.recv(length - len(tmp))
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
        header = self.read(8).decode()
        if header != 'ModBox/M':
            raise Exception('Invalid server header')

    def send_header(self):
        self.write(b'ModBox/m')

    def recv_reverse_header(self):
        header = self.read(8).decode()
        if header != 'ModBox/R':
            raise Exception('Invalid server header')

    def send_reverse_header(self):
        self.write(b'ModBox/r')

    def read_str(self):
        return self.read_until(b'\x00')[:-1].decode()

    def write_str(self, s):
        s += '\x00'
        self.write(bytes(s, 'ascii'))

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
            raise Exception('BLOB is unimplemented')
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
            raise Exception('BLOB is unimplemented')
        else:
            raise Exception('Unknown type: "{}"'.format(tp))

    def invoke(self, func, ls, args, ret):
        if func not in self.command_handles:
            self.write_int(0, size=8, signed=False)
            self.write_str(func)
            handle = self.read_int(size=8, signed=False)
            if handle == 0:
                raise Exception('No such function: "{}"'.format(func))
            self.command_handles[func] = handle

        self.write_int(self.command_handles[func], size=8, signed=False)
        for arg, tp in zip(ls, args):
            self.send_arg(arg, tp)
        exit_code = self.read_int(1, signed=False)
        if exit_code == 0:
            ret_ls = [self.recv_arg(tp) for tp in ret]
            return ret_ls
        else:
            raise Exception(self.read_str())

    def invoke_special(self, func, ls, args, ret):
        self.write_int(self.reserved_handle, size=8, signed=False)
        self.write_str(func)
        for arg, tp in zip(ls, args):
            self.send_arg(arg, tp)
        ret_ls = [self.recv_arg(tp) for tp in ret]
        return ret_ls

    def register_func_provider(self, func, name, args, ret):
        [handle] = self.invoke('core.funcProvider.register', [name, args, ret], 'sss', 'L')
        self.func_provider_names[handle] = name
        self.func_providers[name] = func, args, ret

    def serve_func(self):
        try:
            while True:
                # Wait for a request
                handle = self.read_int(8, signed=False)
                if handle == self.reserved_handle:
                    # exit
                    return

                try:
                    # Parse the request
                    name = self.func_provider_names[handle]
                    func, arg_types, ret_types = self.func_providers

                    # Receive function arguments
                    args = [self.recv_arg(type) for type in arg_types]

                    # Call the function
                    ret = func(*args)
                except BaseException as e:
                    # Something has gone wrong, exit code is not 0
                    self.send_int(1, size=1, signed=False)
                    continue

                # Exit code is 0
                self.send_int(0, size=1, signed=False)
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

def test_func(a, b):
    return [a + b]

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


    print('Registering FuncProvider')
    nc.register_func_provider(test_func, 'module.test', 'ii', 'i')

    print('Invoking core.class.add')
    [handle] = nc.invoke('core.class.add', [0xFFFFFFFFFFFFFFFF, 'Animal', 'names:agei', 'talk,,s'], 'Lsss', 'L')
    print('Got handle: {}'.format(handle))
    print('Invoking core.class.instantiate')
    [object] = nc.invoke('core.class.instantiate', [handle], 'L', 'L')
    print('Got object: {}'.format(object))

    print('Invoking core.class.instance.setString')
    nc.invoke('core.class.instance.setString', [object, 1, 'TesT'], 'LLs', '')
    print('Invoking core.class.instance.getString')
    [s] = nc.invoke('core.class.instance.getString', [object, 1], 'LL', 's')
    print('Got string: {}'.format(s))

    print('Sleeping for 5 seconds')
    time.sleep(5)
    print('Invoking module.test')
    [result] = nc.invoke('module.test', [5, 6], 'ii', 'i')
    print('Got result: {}'.format(result))
    print('Expected result: 11')

    [status] = nc.invoke_special('exit', [], '', 's')

    if status == 'exited':
        print('Exited normally')
    else:
        raise Exception('Unknown exit status: "{}"'.format(status))
if __name__ == '__main__':
    main()
