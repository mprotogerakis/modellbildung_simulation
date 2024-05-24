Setup Embedded Coder:
https://de.mathworks.com/help/ecoder/ug/creating-and-using-host-based-shared-libraries.html

Für den Fall von Apple Mx-Macs anpassen von Chart.mk auf
ARCHS             = arm64e
dann make -f Chart.mk erneut ausführen.

In Python werden die Funktionen der DLL nutzbar via 

    from ctypes import *
    from ctypes import Structure, c_uint8, c_bool, POINTER, c_char_p
    import time

    dylib_file = "Chart.dylib"
    my_functions = CDLL(dylib_file)


Das Interface von Chart.h muss per Hand oder mit Hilfe von ChatGPT übersetzt werden in Python.

Die Struct-Deklarationen sehen dann z.B. so aus:

class DW_Chart_T(Structure):
    _fields_ = [
        ("is_active_c3_Chart", c_uint8),
        ("is_c3_Chart", c_uint8)
    ]

Die globalen Variablen der DLL müssen importiert werden, z.B. :
    Chart_U = ExtU_Chart_T.in_dll(my_functions, 'Chart_U')