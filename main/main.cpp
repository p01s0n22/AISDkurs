#include "SparseMatrix.h"
#include <chrono>

using namespace std::chrono;

int main() {
    SparseMatrix mat;
    int choice;
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
        cout << "Сколько % матрицы будет заполнено: ";
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
    cout << mat;

    // Вычисление определителя
    try {
        auto start = high_resolution_clock::now();
        double determinant = mat.determinant();
        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;
        cout << "Определитель матрицы: " << determinant << endl;
        cout << "\nВремя: " << elapsed.count();
    }
    catch (const std::exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }

    return 0;
}
