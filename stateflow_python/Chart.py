# Importieren der notwendigen Module aus ctypes
from ctypes import *
from ctypes import Structure, c_uint8, c_bool, POINTER, c_char_p
import time

# Laden der dynamischen Bibliothek (dylib)
dylib_file = "Chart.dylib"
my_functions = CDLL(dylib_file)

# Übersetzung der C-Header-Datei (Chart.h) in Python-Strukturen

# Deklaration der Struktur DW_Chart_T
class DW_Chart_T(Structure):
    _fields_ = [
        ("is_active_c3_Chart", c_uint8),  # Status der Chart-Aktivität
        ("is_c3_Chart", c_uint8)          # Status des Chart-Zustands
    ]

# Deklaration der Eingabestruktur ExtU_Chart_T
class ExtU_Chart_T(Structure):
    _fields_ = [
        ("inA", c_bool),  # Eingabe A
        ("inB", c_bool)   # Eingabe B
    ]

# Deklaration der Ausgabestruktur ExtY_Chart_T
class ExtY_Chart_T(Structure):
    _fields_ = [
        ("out", c_bool)   # Ausgabe
    ]

# Deklaration der Struktur tag_RTM_Chart_T für den Modellstatus
class tag_RTM_Chart_T(Structure):
    _fields_ = [
        ("errorStatus", POINTER(c_char_p))  # Zeiger auf den Fehlerstatus
    ]

# Import der globalen Variablen aus der DLL
Chart_DW = DW_Chart_T.in_dll(my_functions, 'Chart_DW')  # Instanz der DW_Chart_T Struktur
Chart_U = ExtU_Chart_T.in_dll(my_functions, 'Chart_U')  # Instanz der ExtU_Chart_T Struktur
Chart_Y = ExtY_Chart_T.in_dll(my_functions, 'Chart_Y')  # Instanz der ExtY_Chart_T Struktur

# Zeiger auf das Modell
RT_MODEL_Chart_T = POINTER(tag_RTM_Chart_T)
Chart_M = RT_MODEL_Chart_T.in_dll(my_functions, 'Chart_M')

# Initialisierung des Modells
my_functions.Chart_initialize()

# Initialisierung der Eingabe- und Ausgabevariablen
Chart_U.inA = False
Chart_U.inB = False
Chart_Y.out = False

# Endlosschleife für die Eingabe und Modellaktualisierung
while True:
    # Ausgabe der aktuellen Eingabe- und Ausgabewerte
    print(f"A:{Chart_U.inA} B:{Chart_U.inB} out:{Chart_Y.out}")
    
    # Benutzerinput für A und B
    Chart_U.inA = input('A:(1/0): ').lower().strip() == '1'
    Chart_U.inB = input('B:(1/0): ').lower().strip() == '1'
    
    # Modellschritt ausführen
    my_functions.Chart_step()
    
    # Kurze Pause
    time.sleep(0.2)

# Modell beenden
my_functions.Chart_terminate()