import asyncio
from ctypes import *
from ctypes import Structure, c_uint8, c_bool, POINTER, c_char_p
import time

class ChartModelWrapper:
    # Laden der dynamischen Bibliothek (dylib)
    _dylib_file = "Chart.dylib"
    _my_functions = CDLL(_dylib_file)

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
    Chart_DW = DW_Chart_T.in_dll(_my_functions, 'Chart_DW')  # Instanz der DW_Chart_T Struktur
    Chart_U = ExtU_Chart_T.in_dll(_my_functions, 'Chart_U')  # Instanz der ExtU_Chart_T Struktur
    Chart_Y = ExtY_Chart_T.in_dll(_my_functions, 'Chart_Y')  # Instanz der ExtY_Chart_T Struktur

    # Zeiger auf das Modell
    RT_MODEL_Chart_T = POINTER(tag_RTM_Chart_T)
    Chart_M = RT_MODEL_Chart_T.in_dll(_my_functions, 'Chart_M')

    @staticmethod
    def initialize():
        ChartModelWrapper._my_functions.Chart_initialize()
        ChartModelWrapper.Chart_U.inA = False
        ChartModelWrapper.Chart_U.inB = False
        ChartModelWrapper.Chart_Y.out = False

    @staticmethod
    def step():
        ChartModelWrapper._my_functions.Chart_step()

    @staticmethod
    async def step_async():
        loop = asyncio.get_event_loop()
        await loop.run_in_executor(None, ChartModelWrapper.step)

    @staticmethod
    def terminate():
        ChartModelWrapper._my_functions.Chart_terminate()

    @staticmethod
    def set_inputs(inA, inB):
        ChartModelWrapper.Chart_U.inA = inA
        ChartModelWrapper.Chart_U.inB = inB

    @staticmethod
    def get_output():
        return ChartModelWrapper.Chart_Y.out

    @staticmethod
    def display_state():
        print(f"A: {ChartModelWrapper.Chart_U.inA} B: {ChartModelWrapper.Chart_U.inB} out: {ChartModelWrapper.Chart_Y.out}")

async def periodic_step(interval):
    while True:
        start_time = time.time()
        await ChartModelWrapper.step_async()
        elapsed = time.time() - start_time
        await asyncio.sleep(max(0, interval - elapsed))

async def main_loop():
    # Initialisierung des Modells
    ChartModelWrapper.initialize()

    # Start des periodischen Tasks
    interval = 0.2  # gewünschtes Intervall in Sekunden
    asyncio.create_task(periodic_step(interval))

    try:
        while True:
            # Ausgabe der aktuellen Eingabe- und Ausgabewerte
            ChartModelWrapper.display_state()

            # Benutzerinput für A und B
            inA = input('A:(1/0): ').lower().strip() == '1'
            inB = input('B:(1/0): ').lower().strip() == '1'
            ChartModelWrapper.set_inputs(inA, inB)

            # Kurze Pause, um die Eingabe zu verarbeiten und periodischen Task auszuführen
            await asyncio.sleep(0.1)
    except KeyboardInterrupt:
        pass
    finally:
        # Modell beenden
        ChartModelWrapper.terminate()

# Ausführung der asynchronen Hauptschleife
asyncio.run(main_loop())
