import subprocess
import matplotlib.pyplot as plt
import pandas as pd

FILE_PATH = "../PO lab1 C++ project/x64/Release/PO lab1 C++ project.exe"
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
            time_single_thread = float(lines[0])
            time_multi_thread = float(lines[1])
            return time_single_thread, time_multi_thread
        else:
            return None
    except Exception as error:
        print(error)
        return None

def main():
    results = []

    for matrix_size in MATRIX_SIZES:
        print(f"{matrix_size}x{matrix_size}:")

        for thread_amount in CORES:
            single_thread_time = []
            multi_thread_time = []

            for _ in range(ITERATIONS):
                time = run_tests(thread_amount, matrix_size)

                if time:
                    single_thread_time.append(time[0])
                    multi_thread_time.append(time[1])

            if single_thread_time and multi_thread_time:
                avg_time_single = sum(single_thread_time) / len(single_thread_time)
                avg_time_multi = sum(multi_thread_time) / len(multi_thread_time)

                speedup = avg_time_single / avg_time_multi if avg_time_multi > 0 else 0

                print(f"Threads = {thread_amount}; Single thread time = {avg_time_single:.4f}s; Multi thread time = {avg_time_multi:.4f}s; Speedup = {speedup}")

                results.append({
                    "matrix_size": matrix_size,
                    "thread_amount": thread_amount,
                    "single_thread_time": single_thread_time,
                    "multi_thread_time": multi_thread_time,
                    "speedup": speedup
                })

    df = pd.DataFrame(results)
    df.to_csv("results.csv", index=False)

    plt.figure(figsize=(12, 8))

    for matrix_size in MATRIX_SIZES:
        subset = df[df["matrix_size"] == matrix_size]
        plt.plot(subset["thread_amount"], subset["speedup"], marker='o', label=f'Size {matrix_size}x{matrix_size}')

    plt.axhline(y=1, color='r', linestyle='--', label='Стандарт прискорення')
    plt.title('Графік показника прискорення')
    plt.xlabel("Кількість потоків")
    plt.ylabel('Коефіцієнт прискорення')
    plt.legend()
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    main()