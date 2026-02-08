// Task:
//      1. Заповнити квадратну матрицю випадковими числами.
//      2. Відобразити верхню половину матриці на нижню дзеркально симетрично відносно горизонтальної осі.
//      3. Вирішити обрану за варіантом задачу, не використовуючи паралелізацію. Заміряти час вирішення задачі,
//         або інші параметри, котрі студент вважає релевантними.
//      4. Вирішити оборану за варіантом задачу, використовуючи паралелізацію. Заміряти час вирішення задачі,
//         або інші параметри, котрі студент вважає релевантними. Обґрунтувати вибір алгоритму паралелізації
//         (розподілення даних між потоками), надати опис та обґрунтування в протоколі роботи.
//      5. Перевірити алгоритм на фіксованих кількостях потоків:
//          • 2-рази меншій, ніж кількість фізичних ядер;
//          • На кількості рівній фізичним ядрам;
//          • На кількості рівній логічних ядрам;
//          • На кількості більшій в 2, 4, 8, 16 разів ніж кількість логічних ядер.
//      6. Повторити пункт 5 з використанням різної розмірності даних.
//      7. Зробити таблиці та графіки залежності часу виконання завдання від кількості потоків для різної розмірності даних.
//         Надати опис графіків, з причинами виникнення отриманих результатів в протоколі роботи.

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <random>

using namespace std;

using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

void no_thread_matrix(vector<vector<int>> &matrix, int matrix_size)
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, 9);
    
    matrix.resize(matrix_size);

    for (int i = 0; i < matrix_size; i++)
    {
        matrix[i].resize(matrix_size);

        for (int j = 0; j < matrix_size; j++)
        {
            matrix[i][j] = dist(gen);
        }
    }
}

void no_thread_edited_matrix(vector<vector<int>>& matrix, vector<vector<int>*>& edited_matrix, int matrix_size)
{
    edited_matrix.resize(matrix_size);

    int middle = matrix_size / 2;
    int j = (matrix_size % 2 == 1) ? 1 : 0;

    for (int i = 0; i < matrix_size; i++)
    {
        edited_matrix[i] = &matrix[(i < middle + j) ? i : matrix_size - i - 1];
    }
}

void thread_function(vector<vector<int>>& matrix, vector<vector<int>*>& mirrored_matrix, int matrix_size, int rows_amount, int start_row_index)
{
    int middle = matrix_size / 2;
    int odd_addition = (matrix_size % 2 == 1) ? 1 : 0;

    thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, 9);
    
    for (int rows = start_row_index; rows < start_row_index + rows_amount; rows++)
    {
        matrix[rows].resize(matrix_size);

        for (int j = 0; j < matrix_size; j++)
        {
            matrix[rows][j] = dist(gen);
        }

        if (rows < middle + odd_addition)
        {
            mirrored_matrix[rows] = &matrix[rows];
            mirrored_matrix[matrix_size - rows - 1] = &matrix[rows];
        }
    }
}

void print_matrixes(vector<vector<int>>& matrix, vector<vector<int>*>& mirrored_matrix, int matrix_size, int middle, bool isOdd)
{
    string repeated_symbol(matrix_size * 2 - 1, '-');
    
    for (int i = 0; i < matrix_size; i++)
    {
        if (i == middle) cout << repeated_symbol << endl;

        for (int j = 0; j < matrix_size; j++)
        {
            cout << matrix[i][j];
            if (j != matrix_size - 1) cout << " ";
        }

        cout << endl;
        if (i == middle && isOdd) cout << repeated_symbol << endl;
    }

    cout << "\n<-- Mirrored matrix -->\n\n";
    for (size_t i = 0; i < matrix_size; ++i)
    {
        vector<int>* row_ptr = mirrored_matrix[i];

        if (i == middle) cout << repeated_symbol << endl;

        for (size_t j = 0; j < row_ptr->size(); ++j)
        {
            cout << (*row_ptr)[j];
            if (j != matrix_size - 1) cout << " ";
        }

        cout << endl;
        if (i == middle && isOdd) cout << repeated_symbol << endl;
    }
}

int main()
{
    vector<vector<int>> matrix;
    int matrix_size = 10000;

    bool isOdd = matrix_size % 2 == 1;
    int middle = matrix_size / 2;

    auto creation_begin = high_resolution_clock::now();
    no_thread_matrix(matrix, matrix_size);
    auto creation_end = high_resolution_clock::now();
    auto no_thread_task_time = duration_cast<nanoseconds>(creation_end - creation_begin);

    vector<vector<int>*> edited_matrix;

    creation_begin = high_resolution_clock::now();
    no_thread_edited_matrix(matrix, edited_matrix, matrix_size);
    creation_end = high_resolution_clock::now();
    no_thread_task_time += duration_cast<nanoseconds>(creation_end - creation_begin);

    if(matrix_size < 20) print_matrixes(matrix, edited_matrix, matrix_size, middle, isOdd);

    cout << "\nTime: " << no_thread_task_time.count() * 1e-9;
    cout << "\n\n-------------------------------------------------------------\n\n";

    // ------------------------------------------

    vector<vector<int>> thread_matrix;
    thread_matrix.resize(matrix_size);

    vector<vector<int>*> mirrored_thread_matrix;
    mirrored_thread_matrix.resize(matrix_size);

    vector<std::thread> threads;

    int thread_amount = 192, start_row_index = 0;
    int rows_per_thread = matrix_size / thread_amount;
    int rows_remainder = matrix_size % thread_amount;

    creation_begin = high_resolution_clock::now();
    for (int thread_id = 0; thread_id < thread_amount; thread_id++)
    {
        int remainder = (rows_remainder > 0) ? 1 : 0;
        int rows_amount = rows_per_thread + remainder;
        
        threads.emplace_back(thread_function, std::ref(thread_matrix), std::ref(mirrored_thread_matrix), matrix_size, rows_amount, start_row_index);

        start_row_index += rows_amount;
        
        if(rows_remainder > 0) rows_remainder--;
    }

    for (auto& thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
    creation_end = high_resolution_clock::now();
    auto thread_task_time = duration_cast<nanoseconds>(creation_end - creation_begin);

    if (matrix_size < 20) print_matrixes(matrix, edited_matrix, matrix_size, middle, isOdd);

    cout << "\nTime: " << thread_task_time.count() * 1e-9;

    return 0;
}
