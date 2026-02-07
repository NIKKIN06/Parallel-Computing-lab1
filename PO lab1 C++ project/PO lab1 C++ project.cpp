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
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>

using namespace std;

using chrono::nanoseconds;
using chrono::duration_cast;
using chrono::high_resolution_clock;

void no_thread_matrix(vector<vector<int>> &matrix, int matrix_size)
{
    matrix.resize(matrix_size);

    for (int i = 0; i < matrix_size; i++)
    {
        matrix[i].resize(matrix_size);

        for (int j = 0; j < matrix_size; j++)
        {
            matrix[i][j] = rand() % 10;
        }
    }
}

void no_thread_edited_matrix(vector<vector<int>>& matrix, vector<vector<int>*>& edited_matrix, int matrix_size)
{
    edited_matrix.resize(matrix_size);

    int middle = (int)round(matrix_size / 2);
    int j = (matrix_size % 2 == 1) ? 0 : 1;

    for (int i = 0; i < matrix_size; i++)
    {
        edited_matrix[i] = &matrix[(i > middle - j) ? matrix_size - i - 1 : i];
    }
}

int main()
{
    srand(time(0));

    vector<vector<int>> matrix;
    int matrix_size = 10000;

    bool isOdd = matrix_size % 2 == 1;
    int middle = (int)round(matrix_size / 2);
    string repeated_symbol(matrix_size * 2 - 1, '-');

    auto creation_begin = high_resolution_clock::now();
    no_thread_matrix(matrix, matrix_size);
    auto creation_end = high_resolution_clock::now();
    auto no_thread_task_time = duration_cast<nanoseconds>(creation_end - creation_begin);

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

    vector<vector<int>*> edited_matrix;

    creation_begin = high_resolution_clock::now();
    no_thread_edited_matrix(matrix, edited_matrix, matrix_size);
    creation_end = high_resolution_clock::now();
    no_thread_task_time += duration_cast<nanoseconds>(creation_end - creation_begin);

    cout << "\n<-- Mirrored matrix -->\n\n";
    for (size_t i = 0; i < edited_matrix.size(); ++i)
    {
        vector<int>* row_ptr = edited_matrix[i];

        if (i == middle) cout << repeated_symbol << endl;

        for (size_t j = 0; j < row_ptr->size(); ++j)
        {
            cout << (*row_ptr)[j];
            if (j != matrix_size - 1) cout << " ";
        }

        cout << endl;
        if (i == middle && isOdd) cout << repeated_symbol << endl;
    }

    cout << "\nTime: " << no_thread_task_time.count() * 1e-9;

    return 0;
}
