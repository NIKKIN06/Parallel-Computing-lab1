import subprocess
import matplotlib.pyplot as plt
import pandas as pd

FILE_PATH = "./PO lab1 C++ project.exe"
CORES = [4, 8, 12, 24, 48, 96, 192]     # 8 physical cores and 12 logical cores
MATRIX_SIZES = [3, 9, 18, 324, 1000, 5000, 10000]
ITERATIONS = 3

def run_tests(thread_amount, matrix_size):
    try:
        time_result = subprocess.run(
            [FILE_PATH, str(matrix_size), str(thread_amount)],
            capture_output=True, text=True, check=True
        )

        lines = time_result.stdout.splitlines()

        if len(lines) >= 2:
            time_single_thread = int(lines[0])
            time_multi_thread = int(lines[1])
            return time_single_thread, time_multi_thread
        else:
            return None
    except Exception as error:
        print(error)
        return None