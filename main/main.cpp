#include"SparseMatrix.h"

int main() {
    SparseMatrix mat;
    int choice;
    system("chcp 1251");
    system("cls");
    //SparseMatrix mat(5);
    //mat.add(10, 1, 2);
    //mat.add(20, 2, 3);
    //mat.add(30, 1, 2); // Обновляет элемент (1, 2) на 30
    //cout << mat;

    cout << "Выберите способ ввода матрицы:" << endl;
    cout << "1. Ввод полной матрицы вручную" << endl;
    cout << "2. Ввод матрицы из файла" << endl;
    cin >> choice;

    if (choice == 1) {
        mat.inputMatrix();
    }
    else if (choice == 2) {
        char filename[100];
        cout << "Введите имя файла: ";
        cin >> filename;
        mat.inputFromFile(filename);
    }
    else {
        cout << "Неверный выбор!" << endl;
        return 1;
    }

    cout << "Ваша разреженная матрица:" << endl;
    cout << mat;

    return 0;
}