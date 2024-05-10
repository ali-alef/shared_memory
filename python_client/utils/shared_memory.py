import ctypes

import settings

shared_lib = ctypes.CDLL(settings.DLL_PATH)

shared_lib.create_shared_memory.restype = ctypes.c_int
shared_lib.write_to_shared_memory.argtypes = [ctypes.c_int, ctypes.c_char_p]
shared_lib.read_from_shared_memory.argtypes = [ctypes.c_int]
shared_lib.read_from_shared_memory.restype = ctypes.c_char_p


def create_shared_memory():
    return shared_lib.create_shared_memory()


def write_to_shared_memory(shm_fd, data):
    if type(data) is str:
        data = str.encode(data)
    shared_lib.write_to_shared_memory(shm_fd, data)


def read_from_shared_memory(shm_fd):
    return shared_lib.read_from_shared_memory(shm_fd).decode()
