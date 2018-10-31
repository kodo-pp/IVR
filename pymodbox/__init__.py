#!/usr/bin/env false
# -*- coding: utf-8 -*-

import sys
import socket
import base64
import time
import re
import traceback
import os
from threading import Lock, Thread

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
            received = self.socket.recv(1024)
            if len(received) == 0:
                self.socket.close()
                raise IOError('Connection closed')
            self.buff += received
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
    def __init__(self, ip, port, logger):
        Netcat.__init__(self, ip, port)
        self.logger = logger
        self.func_provider_names = {}
        self.func_providers = {}

    def recv_header(self):
        self.logger.vlog('Reading host header')
        header = self.read(8).decode()
        if header != 'ModBox/M':
            raise Exception('Invalid server header')

    def send_header(self):
        self.logger.vlog('Sending module header')
        self.write(b'ModBox/m')

    def recv_reverse_header(self):
        self.logger.vlog('Reading reverse host header')
        header = self.read(8).decode()
        if header != 'ModBox/R':
            raise Exception('Invalid server header')

    def send_reverse_header(self):
        self.logger.vlog('Sending reverse module header')
        self.write(b'ModBox/r')

    def read_str(self):
        self.logger.vvlog('{}: Reading string...'.format(id(self)))
        s = self.read_until(b'\x00')[:-1].decode()
        self.logger.vvlog('{}: Reading string: "{}"'.format(id(self), s))
        return s

    def write_str(self, s):
        if s is None:
            raise Exception('Attempted to send a None value')
        self.logger.vvlog('{}: Writing string: "{}"'.format(id(self), s))
        s = str(s)
        s += '\x00'
        self.write(bytes(s, 'ascii'))

    def unblobify(self, blob):
        self.logger.vvlog('unblobify: {}'.format(repr(blob)))
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
        self.logger.vvlog('send_arg: self = {}, arg = {}, tp = {}'.format(id(self), arg, tp))
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
        self.logger.vvlog('blobify: {}'.format(repr(blob)))
        return blob

    def invoke(self, func, ls, args, ret):
        self.logger.vlog('Invoking {}({})...'.format(func, ', '.join(map(str, ls))))
        self.write_str(func)
        for arg, tp in zip(ls, args):
            self.send_arg(arg, tp)
        exit_code = int(self.read_str())
        if exit_code == 0:
            ret_ls = [self.recv_arg(tp) for tp in ret]
            self.logger.vlog('... = {}'.format(ret_ls))
            return ret_ls
        else:
            error = self.read_str()
            self.logger.vlog('... error: {}'.format(error))
            raise Exception(error)

    def register_func_provider(self, storage, func, name, args, ret):
        self.logger.vlog('Registering FuncProvider: "{}" ({}) -> {}'.format(name, args, ret))
        self.invoke('core.funcProvider.register', [name, args, ret], 'sss', '')
        storage.func_providers[name] = func, args, ret

    def serve_func(self):
        try:
            self.logger.vlog('Serving')
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

                    # Call the function
                    ret = func(*args)
                except BaseException as e:
                    # Something has gone wrong, exit code is not 0
                    self.logger.log('Exception at module function:')
                    traceback.print_exc()
                    self.write_str(1)
                    continue

                # Exit code is 0
                self.write_str(0)
                for type, val in zip(ret_types, ret):
                    self.send_arg(val, type)
        except BaseException as e:
            self.logger.log('Exception occured at serve_func: ' + str(e))
            os._exit(1)

    def spawn_serving_thread(self):
        self.serving_thread = Thread(target=self.serve_func)
        self.serving_thread.start()

    def join_serving_thread(self):
        self.serving_thread.join()

    def get_class(self, classname):
        return Class(self, classname)

    def add_class(self, classname, members, methods, parent=''):
        self.logger.vlog('Adding class "{}"'.format(classname))
        members_string = ':'.join(map(lambda pair: ''.join(pair), members))
        methods_string = ':'.join(map(lambda tup: ','.join(tup), methods))
        self.invoke('core.class.add', [parent, classname, members_string, methods_string], 'ssss', '')
        return Class(self, classname)

    def instantiate_class(self, classname):
        self.logger.vlog('Instantiating class {}'.format(classname))
        return self.invoke('core.class.instantiate', [classname], 's', 'u')[0]


class Module:
    def __init__(self, module_name):
        self.module_name = module_name
        self.VERBOSE = True
        self.VERY_VERBOSE = False
        self.call_lock = Lock()

        self.main_port, self.reverse_port = self.portinfo(sys.argv)
        self.nc = Modcat('localhost', self.main_port, logger=self)
        self.rnc = Modcat('localhost', self.reverse_port, logger=self)

        self.nc.recv_header()
        self.nc.send_header()
        self.nc.write_str(module_name)
        self.rnc.recv_reverse_header()
        self.rnc.send_reverse_header()
        self.rnc.write_str(module_name)
        self.rnc.spawn_serving_thread()

    def register_func_provider(self, func, command, args, ret):
        self.nc.register_func_provider(self.rnc, func.__get__(self), command, args, ret)

    def invoke(self, *args, **kwargs):
        with self.call_lock:
            return self.nc.invoke(*args, **kwargs)

    def _get_log_time(self):
        return time.strftime('%02d.%02m.%Y %02H:%02M:%02S')

    def log(self, text):
        print("[MODLOG ({}) {}] ".format(self.module_name, self._get_log_time()) + str(text))
        sys.stdout.flush()

    def vlog(self, text):
        if self.VERBOSE:
            self.log(text)

    def vvlog(self, text):
        if self.VERY_VERBOSE:
            self.log(text)

    def portinfo(self, argv):
        main_port = None
        reverse_port = None
        self.vlog(argv)
        for arg in argv:
            if re.match(r'^--main-port=[0-9]*$', arg) is not None:
                main_port = int(arg.split('=')[1])
            if re.match(r'^--reverse-port=[0-9]*$', arg) is not None:
                reverse_port = int(arg.split('=')[1])
        if main_port is None:
            raise ValueError('Main port information not provided')
        if reverse_port is None:
            raise ValueError('Reverse port information not provided')
        return main_port, reverse_port

    def ready(self):
        self.invoke('module.ready', [], '', '')
