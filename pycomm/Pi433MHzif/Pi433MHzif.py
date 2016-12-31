# -*- coding: utf-8 -*-
#########################################################
# SERVICE : Pi433MHzif.py                              #
#           Python client for 433 MHz daemon            #
#           I. Helwegen 2016                            #
#########################################################

####################### IMPORTS #########################
import ctypes
import os.path

#########################################################

####################### GLOBALS #########################
libfile = "libPi433MHzif.so"
deflibloc1 = "/usr/lib"
deflibloc2 = "/usr/local/lib"

def find_lib_loc():
    fname = deflibloc1 + "/" + libfile
    if os.path.isfile(fname):
        return fname
    fname = deflibloc2 + "/" + libfile
    if os.path.isfile(fname):
        return fname
    fname = "./" + libfile
    if os.path.isfile(fname):
        return fname
    fname = "../" + libfile
    if os.path.isfile(fname):
        return fname
    print ("ERROR: C library not found: " + libfile)
    exit(1);

lib = ctypes.CDLL(find_lib_loc())
lib._Pi433MHzif_WriteMessage.argtypes = (ctypes.c_void_p,ctypes.POINTER(ctypes.c_int), ctypes.c_int)
lib._Pi433MHzif_ReadMessage.argtypes = (ctypes.c_void_p,ctypes.POINTER(ctypes.c_int), ctypes.c_int)

#########################################################
# Class : Pi433MHzif                                    #
#########################################################
class Pi433MHzif(object):
    def __init__(self):
        self.obj = lib._Pi433MHzif()

    def __exit__(self, exc_type, exc_value, traceback):
        lib._Pi433MHzif_delete(self.obj)

    def ReadMessage(self, maxsize):
        array = []
        array_type = ctypes.c_int * maxsize
        arr = array_type()
        result = lib._Pi433MHzif_ReadMessage(self.obj, arr, maxsize)
        for i in range(0, result):
            array.append(int(arr[i]))
        return int(result), array

    def WriteMessage(self, array, size):
        array_type = ctypes.c_int * size
        arr = array_type()
        for i in range(0, size):
            arr[i] = ctypes.c_int(array[i]);
        result = lib._Pi433MHzif_WriteMessage(self.obj, arr, ctypes.c_int(size))
        return int(result)
