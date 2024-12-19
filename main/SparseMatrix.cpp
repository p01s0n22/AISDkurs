﻿#include "SparseMatrix.h"

SparseMatrix::SparseMatrix() {
    hRow = nullptr;
    hCol = nullptr;
    size = 0;
}

SparseMatrix::SparseMatrix(size_t n) {
    size = n;
    hRow = new NODE * [size];
    hCol = new NODE * [size];
    for (size_t i = 0; i < size; i++) {
        hRow[i] = nullptr;
        hCol[i] = nullptr;
    }
}

SparseMatrix::~SparseMatrix() {
    clear();
}

void SparseMatrix::clear() {
    if (hRow != nullptr) {
        for (int i = 0; i < size; i++) {
            NODE* current = hRow[i];
            while (current != nullptr) {
                NODE* temp = current;
                current = current->nextright;
                cout << "Deleting node at (" << temp->row << ", " << temp->col << ")" << endl;  // Логируем удаление
                delete temp;
            }
        }
        delete[] hRow;  // Освобождаем память для строк
        hRow = nullptr;  // Обнуляем указатель, чтобы избежать повторного удаления
    }

    if (hCol != nullptr) {
        delete[] hCol;  // Освобождаем память для столбцов
        hCol = nullptr;  // Обнуляем указатель, чтобы избежать повторного удаления
    }

    size = 0;  // Обнуляем размер
}





SparseMatrix::SparseMatrix(const SparseMatrix& other) {
    size = other.size;
    hRow = new NODE * [size]();
    hCol = new NODE * [size]();
    copyFrom(other);
}

SparseMatrix& SparseMatrix::operator=(const SparseMatrix& other) {
    if (this == &other) return *this;

    clear();       // Удаляем текущую память
    size = other.size;
    hRow = new NODE * [size]();
    hCol = new NODE * [size]();
    copyFrom(other); // Копируем новые данные

    return *this;
}

void SparseMatrix::copyFrom(const SparseMatrix& other) {
    clear();
    size = other.size;

    hRow = new NODE * [size]();
    hCol = new NODE * [size]();

    for (int i = 0; i < size; i++) {
        NODE* current = other.hRow[i];
        while (current != nullptr) {
            add(current->data, current->row, current->col);
            current = current->nextright;
        }
    }
}

void SparseMatrix::add(double val, int row, int col) {
    if (row < 0 || col < 0 || row >= size || col >= size)
        throw std::out_of_range("Invalid row or column index");

    // Работа с `hRow`
    NODE** rowPtr = &hRow[row];
    while (*rowPtr != nullptr && (*rowPtr)->col < col)
        rowPtr = &(*rowPtr)->nextright;

    if (*rowPtr != nullptr && (*rowPtr)->col == col) {
        if (val == 0) { // Удаление узла
            NODE* temp = *rowPtr;
            *rowPtr = (*rowPtr)->nextright;
            delete temp;
        }
        else {
            (*rowPtr)->data = val;
        }
    }
    else if (val != 0) {
        NODE* newNode = new NODE(val, row, col);
        newNode->nextright = *rowPtr;
        *rowPtr = newNode;

        // Работа с `hCol`
        NODE** colPtr = &hCol[col];
        while (*colPtr != nullptr && (*colPtr)->row < row)
            colPtr = &(*colPtr)->nextdown;

        newNode->nextdown = *colPtr;
        *colPtr = newNode;
    }
}



double SparseMatrix::get(int row, int col) const {
    if (row >= size || col >= size || row < 0 || col < 0) {
        cout << "Ошибка в координатах" << endl;
        return 0;
    }

    NODE* current = hRow[row];

    // Если строка пуста, сразу возвращаем 0
    if (!current) return 0;

    // Обходим элементы в строке, проверяя их столбцы
    while (current != nullptr) {
        if (current->col == col) {
            return current->data;  // Найден элемент в нужной ячейке
        }
        current = current->nextright;
    }

    return 0;  // Если элемента с таким индексом нет
}



void SparseMatrix::inputMatrix() {
    int n;
    cout << "Введите размер матрицы: ";
    cin >> n;
    if (n <= 0) {
        cout << "Ошибка: Размер матрицы должен быть положительным числом!" << endl;
        return;
    }
    *this = SparseMatrix(n);


    cout << "Введите матрицу построчно (включая нули):" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int value;
            cin >> value;
            if (value != 0) {
                add(value, i, j);
            }
        }
    }
}

SparseMatrix SparseMatrix::operator+(const SparseMatrix& other) const {
    if (size != other.size) {
        throw std::invalid_argument("Matrix sizes do not match");
    }

    SparseMatrix result(size);

    for (int i = 0; i < size; i++) {
        NODE* thisRow = hRow[i];
        NODE* otherRow = other.hRow[i];

        while (thisRow != nullptr || otherRow != nullptr) {
            if (thisRow == nullptr) {
                result.add(otherRow->data, otherRow->row, otherRow->col);
                otherRow = otherRow->nextright;
            }
            else if (otherRow == nullptr) {
                result.add(thisRow->data, thisRow->row, thisRow->col);
                thisRow = thisRow->nextright;
            }
            else if (thisRow->col == otherRow->col) {
                result.add(thisRow->data + otherRow->data, thisRow->row, thisRow->col);
                thisRow = thisRow->nextright;
                otherRow = otherRow->nextright;
            }
            else if (thisRow->col < otherRow->col) {
                result.add(thisRow->data, thisRow->row, thisRow->col);
                thisRow = thisRow->nextright;
            }
            else {
                result.add(otherRow->data, otherRow->row, otherRow->col);
                otherRow = otherRow->nextright;
            }
        }
    }

    return result;
}



SparseMatrix SparseMatrix::operator-(const SparseMatrix& other) const {
    if (size != other.size) {
        throw std::invalid_argument("Matrix sizes do not match");
    }

    SparseMatrix result(size);

    for (int i = 0; i < size; i++) {
        NODE* thisRow = hRow[i];
        NODE* otherRow = other.hRow[i];

        while (thisRow != nullptr || otherRow != nullptr) {
            if (thisRow == nullptr) {
                result.add(-otherRow->data, otherRow->row, otherRow->col);
                otherRow = otherRow->nextright;
            }
            else if (otherRow == nullptr) {
                result.add(thisRow->data, thisRow->row, thisRow->col);
                thisRow = thisRow->nextright;
            }
            else if (thisRow->col == otherRow->col) {
                result.add(thisRow->data - otherRow->data, thisRow->row, thisRow->col);
                thisRow = thisRow->nextright;
                otherRow = otherRow->nextright;
            }
            else if (thisRow->col < otherRow->col) {
                result.add(thisRow->data, thisRow->row, thisRow->col);
                thisRow = thisRow->nextright;
            }
            else {
                result.add(-otherRow->data, otherRow->row, otherRow->col);
                otherRow = otherRow->nextright;
            }
        }
    }

    return result;
}



SparseMatrix SparseMatrix::operator*(const SparseMatrix& other) const {
    if (size != other.size) {
        throw std::invalid_argument("Matrix sizes do not match");
    }

    SparseMatrix result(size);

    for (int i = 0; i < size; i++) {
        NODE* thisRow = hRow[i];


        while (thisRow != nullptr) {
            NODE* otherCol = other.hCol[thisRow->col];
            while (otherCol != nullptr) {
                result.add(thisRow->data * otherCol->data, i, otherCol->row);
                otherCol = otherCol->nextdown;
            }
            thisRow = thisRow->nextright;
        }
    }

    return result;
}



void SparseMatrix::swapRows(int row1, int row2) {
    if (row1 < 0 || row1 >= size || row2 < 0 || row2 >= size) {
        throw std::out_of_range("Row indices are out of range");
    }

    // Меняем указатели в строках
    NODE* temp = hRow[row1];
    hRow[row1] = hRow[row2];
    hRow[row2] = temp;

    // Поменять местами указатели в столбцах тоже
    for (int i = 0; i < size; i++) {
        NODE* current = hCol[i];
        while (current != nullptr) {
            if (current->row == row1) {
                current->row = row2;
            }
            else if (current->row == row2) {
                current->row = row1;
            }
            current = current->nextdown;
        }
    }
}


SparseMatrix SparseMatrix::submatrix(int delRow, int delCol) const {
    if (delRow < 0 || delRow >= size || delCol < 0 || delCol >= size) {
        throw std::out_of_range("Row or column index out of range");
    }

    SparseMatrix result(size - 1);

    for (int i = 0; i < size; i++) {
        if (i == delRow) continue;
        NODE* rowPtr = hRow[i];

        while (rowPtr != nullptr) {
            if (rowPtr->col == delCol) {
                rowPtr = rowPtr->nextright;
                continue;
            }

            result.add(rowPtr->data, i, rowPtr->col);
            rowPtr = rowPtr->nextright;
        }
    }

    return result;
}


double SparseMatrix::determinant() const {
    if (size == 0) {
        cout << "Матрица пуста." << endl;
        return 0;
    }

    // Копируем матрицу в массив для выполнения операций
    double** matrix = new double* [size];
    for (int i = 0; i < size; ++i) {
        matrix[i] = new double[size];
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = get(i, j);
        }
    }

    double det = 1.0; // Начальное значение определителя
    int swapCount = 0; // Счетчик перестановок строк

    cout << "Начинаем вычисление детерминанта..." << endl;

    for (int col = 0; col < size; ++col) {
        cout << "Обрабатываем столбец " << col << endl;

        // Вывод текущего столбца
        for (int row = 0; row < size; ++row) {
            cout << "Элемент в строке " << row << ", столбце " << col << ": " << matrix[row][col] << endl;
        }

        // Поиск строки с максимальным абсолютным значением в текущем столбце
        int maxRow = col;
        double maxVal = abs(matrix[col][col]);
        for (int row = col + 1; row < size; ++row) {
            if (abs(matrix[row][col]) > maxVal) {
                maxRow = row;
                maxVal = abs(matrix[row][col]);
            }
        }

        cout << "Максимальный элемент в столбце " << col << " находится в строке " << maxRow
            << " со значением " << matrix[maxRow][col] << endl;

        // Если максимальный элемент равен 0, определитель равен 0
        if (maxVal == 0) {
            cout << "Все элементы в столбце " << col << " нулевые, определитель равен 0." << endl;
            for (int i = 0; i < size; ++i) delete[] matrix[i];
            delete[] matrix;
            return 0;
        }

        // Перестановка строк, если максимальный элемент не в текущей строке
        if (maxRow != col) {
            swap(matrix[maxRow], matrix[col]);
            swapCount++;
            cout << "Меняем строки местами: строка " << col << " и строка " << maxRow << endl;
        }

        // Пивотное значение
        double pivot = matrix[col][col];
        det *= pivot; // Умножаем на диагональный элемент
        cout << "Пивотное значение для столбца " << col << " равно " << pivot << endl;


        // Преобразуем строки ниже текущей
        for (int row = col + 1; row < size; ++row) {
            double coeff = matrix[row][col] / pivot;
            matrix[row][col] = 0; // Элемент ниже диагонали становится 0
            for (int j = col + 1; j < size; ++j) {
                matrix[row][j] -= coeff * matrix[col][j];
            }
            cout << "Вычисляем коэффициент для строки " << row << ": " << coeff << endl;
            for (int j = col + 1; j < size; ++j) {
                cout << "Обновляем элемент в строке " << row << ", столбце " << j
                    << " на " << matrix[row][j] << endl;
            }
        }
        cout << "После обработки столбца " << col << ", детерминант: " << det << endl;
    }

    // Учет перестановок строк
    if (swapCount % 2 != 0) {
        det = -det;
        cout << "Нечетное количество перестановок строк, изменяем знак детерминанта." << endl;
    }

    // Освобождение памяти
    for (int i = 0; i < size; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;

    cout << "Конечный определитель: " << det << endl;

    // Проверка малого значения
    if (abs(det) < 1e-12) {
        cout << "Определитель слишком мал, округляем до 0." << endl;
        return 0;
    }

    return det;
}




void SparseMatrix::generateRandomMatrix(size_t n, int density) {
    size = n;
    hRow = new NODE * [n];
    hCol = new NODE * [n];
    for (int i = 0; i < n; i++) {
        hRow[i] = nullptr;
        hCol[i] = nullptr;
    }

    srand(time(0));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (rand() % 100 < density) { // С вероятностью density заполняем элементы
                double value = (rand() % 100) + 1;
                add(value, i, j);
            }
        }
    }
}



ostream& operator<<(ostream& os, const SparseMatrix& matrix) {
    for (int i = 0; i < matrix.size; i++) {
        SparseMatrix::NODE* current = matrix.hRow[i]; // Объявляем указатель на текущую строку
        for (int j = 0; j < matrix.size; j++) {
            if (current && current->col == j) { // Если нашли элемент в текущей строке
                os << setw(5) << current->data;
                current = current->nextright; // Переходим к следующему элементу в строке
            }
            else {
                os << setw(5) << 0; // Если элемент отсутствует, выводим 0
            }
        }
        os << endl; // Переход на новую строку после вывода всех столбцов
    }
    return os;
}
