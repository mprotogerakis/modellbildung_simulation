
from ctypes import *
from ctypes import Structure, c_uint8, c_bool, POINTER, c_char_p
import time

dylib_file = "Chart.dylib"
my_functions = CDLL(dylib_file)

# FROM Chart.h

class DW_Chart_T(Structure):
    _fields_ = [
        ("is_active_c3_Chart", c_uint8),
        ("is_c3_Chart", c_uint8)
    ]

class ExtU_Chart_T(Structure):
    _fields_ = [
        ("inA", c_bool),
        ("inB", c_bool)
    ]

class ExtY_Chart_T(Structure):
    _fields_ = [
        ("out", c_bool)
    ]

class tag_RTM_Chart_T(Structure):
    _fields_ = [
        ("errorStatus", POINTER(c_char_p))
    ]

Chart_DW = DW_Chart_T.in_dll(my_functions, 'Chart_DW') #This method returns a ctypes type instance exported by a shared library. name is the name of the symbol that exports the data, library is the loaded shared library.
Chart_U = ExtU_Chart_T.in_dll(my_functions, 'Chart_U')
Chart_Y = ExtY_Chart_T.in_dll(my_functions, 'Chart_Y')

RT_MODEL_Chart_T = POINTER(tag_RTM_Chart_T)
Chart_M = RT_MODEL_Chart_T.in_dll(my_functions, 'Chart_M')

#/* Initialize model */
my_functions.Chart_initialize()

Chart_U.inA = False
Chart_U.inB = False
Chart_Y.out = False

while True:
    print(f"A:{Chart_U.inA} B:{Chart_U.inB} out:{Chart_Y.out}")
    Chart_U.inA = input('A:(1/0): ').lower().strip() == '1'
    Chart_U.inB = input('B:(1/0): ').lower().strip() == '1'
    my_functions.Chart_step()
    time.sleep(0.2)


my_functions.Chart_terminate()