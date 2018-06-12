#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket
import base64
import math
import time

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
        self.write_str(func)
        for arg, tp in zip(ls, args):
            self.send_arg(arg, tp)
        ret_ls = [self.recv_arg(tp) for tp in ret]
        return ret_ls

def main():
    nc = Modcat('localhost', 44145)
    nc.recv_header()
    nc.send_header()

    module_name = 'Python test module'
    nc.write_str(module_name)

    a = 245
    b = 3712
    s1 = 'This is a t'
    s2 = 'est'

    c, s = nc.invoke('test', [a, b, s1, s2], 'iiss', 'ls')

    print('c = {}, s = "{}", len(s) = {}'.format(c, s, len(s)))

    cube = nc.invoke('graphics.createCube', [], '', 'L')[0]

    i = 0
    while True:
        pos = math.cos(i) * 20
        nc.invoke('graphics.moveObject', [cube, pos, pos, pos], 'LFFF', '')
        time.sleep(0.05)
        i += 0.05

    nc.invoke('exit', [], '', '')
if __name__ == '__main__':
    main()
