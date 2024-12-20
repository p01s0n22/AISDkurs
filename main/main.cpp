#include "SparseMatrix.h"
#include <chrono>
#include <iostream>
#include <iomanip>

using namespace std::chrono;
using namespace std;

int main() {
    SparseMatrix mat;
    int choice;

    // Устанавливаем кодировку для корректного отображения русских символов
    system("chcp 1251");
    system("cls");

    cout << "Выберите способ ввода матрицы:" << endl;
    cout << "1. Ввод полной матрицы вручную" << endl;
    cout << "2. Генерация случайной матрицы" << endl;
    cin >> choice;

    if (choice == 1) {
        mat.inputMatrix();
    }
    else if (choice == 2) {
        size_t size;
        int density;

        cout << "Введите размер матрицы: ";
        cin >> size;
        cout << "Сколько % матрицы будет заполнено (от 1 до 100): ";
        cin >> density;

        try {
            mat.generateRandomMatrix(size, density);
        }
        catch (const std::exception& e) {
            cerr << "Ошибка: " << e.what() << endl;
            return 1;
        }
    }
    else {
        cout << "Неверный выбор!" << endl;
        return 1;
    }

    cout << "Ваша разреженная матрица:" << endl;
    cout << mat << endl;

    // Вычисление определителя
    try {
        auto start = high_resolution_clock::now();
        double determinant = mat.determinant();
        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;

        cout << "Определитель матрицы: " << determinant << endl;
        cout << "Время вычисления: " << elapsed.count() << " секунд" << endl;
    }
    catch (const std::exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }

    return 0;
}
