## Schritt 01: Setup Embedded Coder:

![Setup Embedded Coder](https://www.dropbox.com/s/h7hyrzq3sl9q4e4/Bildschirmfoto%202024-05-25%20um%2007.26.47.png?dl=1)



## Schritt 1: ERT_Shrlib System-Target-Datei auswählen
Durch die Auswahl der `ert_shrlib.tlc` System-Target-Datei wird der Build-Prozess so konfiguriert, dass eine Shared-Library-Version Ihres Modellcodes im aktuellen Arbeitsverzeichnis erstellt wird. Diese Auswahl ändert den vom Code-Generator für Ihr Modell produzierten Code nicht.

## Schritt 2: Code für das Subsystem generieren.

![Code für das Subsystem generieren]https://www.dropbox.com/s/91kgchl8n4illos/Bildschirmfoto%202024-05-25%20um%2007.27.38.png?dl=1]


## (Schritt 3: Anpassungen für Apple Mx-Macs)
Falls Sie einen Apple Mx-Mac verwenden, passen Sie die Datei `Chart.mk` an:
```makefile
ARCHS = arm64e
```

dann `make -f Chart.mk` erneut ausführen.

## Schritt 4: Den Python Teil schreiben.

Einfache Beispiele zur Nutzung von C-Funktionen via `ctypes` in python im Allgemeinen finden sich: https://pgi-jcns.fz-juelich.de/portal/pages/using-c-from-python.html


In Python werden die Funktionen der Simulink-generierten DLL nutzbar via 

    from ctypes import *
    from ctypes import Structure, c_uint8, c_bool, POINTER, c_char_p
    import time

    dylib_file = "Chart.dylib"
    my_functions = CDLL(dylib_file)


## Schritt 5: Das Interface aus dem Header-File nach Python übersetzen.
Das Interface von `Chart.h` muss per Hand oder mit Hilfe von ChatGPT übersetzt werden in Python.

Die Struct-Deklarationen sehen dann z.B. so aus:

    class DW_Chart_T(Structure):
        _fields_ = [
            ("is_active_c3_Chart", c_uint8),
            ("is_c3_Chart", c_uint8)
        ]

Entsprechend https://docs.python.org/3/library/ctypes.html#ctypes._CData.in_dll werden die globalen Variablen der DLL importiert im `in_dll`, z.B.:
    
    // Diese funktion importiert die gobale Variable Chart_U des Typs 
    ExtU_Chart_T aus der DLL
    Chart_U = ExtU_Chart_T.in_dll(my_functions, 'Chart_U')
    
    
    