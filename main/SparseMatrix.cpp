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
                delete temp;
            }
        }
        delete[] hRow;  
        hRow = nullptr;  
    }

    if (hCol != nullptr) {
        delete[] hCol; 
        hCol = nullptr; 
    }

    size = 0; 
}

SparseMatrix::SparseMatrix(const SparseMatrix& other) {
    size = other.size;
    hRow = new NODE * [size]();
    hCol = new NODE * [size]();
    copyFrom(other);
}

SparseMatrix& SparseMatrix::operator=(const SparseMatrix& other) {
    if (this == &other) return *this;

    clear();       
    size = other.size;
    hRow = new NODE * [size]();
    hCol = new NODE * [size]();
    copyFrom(other); 

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

    NODE** rowPtr = &hRow[row];
    while (*rowPtr != nullptr && (*rowPtr)->col < col)
        rowPtr = &(*rowPtr)->nextright;

    if (*rowPtr != nullptr && (*rowPtr)->col == col) {
        if (val == 0) { 
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

        NODE** colPtr = &hCol[col];
        while (*colPtr != nullptr && (*colPtr)->row < row)
            colPtr = &(*colPtr)->nextdown;

        newNode->nextdown = *colPtr;
        *colPtr = newNode;
    }
}



int SparseMatrix::get(int row, int col) const {
    if (row >= size || col >= size || row < 0 || col < 0) {
        cout << "Ошибка в координатах" << endl;
        return 0;
    }

    NODE* current = hRow[row];
    if (!current) return 0;

    while (current != nullptr) {
        if (current->col == col) {
            return current->data;  
        }
        current = current->nextright;
    }
    return 0;  
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

    NODE* temp = hRow[row1];
    hRow[row1] = hRow[row2];
    hRow[row2] = temp;

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

double SparseMatrix::determinant() {
    double det = 1;  // Переменная для хранения определителя
    int swapCount = 0;  // Счетчик перестановок строк
    int scalingFactor = 1; // Накопление знаменателей для масштабирования (если матрица имеет элементы, которые нужно учитывать при вычитаниях)

    // Проходим по всем строкам и столбцам для приведения матрицы к верхнетреугольному виду
    for (int i = 0; i < size; ++i) {
        NODE* pivotElement = getElement(i, i);  // Получаем ведущий элемент для строки i (элемент на главной диагонали)

        // Если ведущий элемент равен нулю, ищем строку ниже для обмена
        if (pivotElement == nullptr || pivotElement->data == 0) {
            bool found = false;  // Флаг для поиска строки с ненулевым элементом
            // Ищем ненулевой элемент в столбце i (смотрим все строки ниже)
            for (int j = i + 1; j < size; ++j) {
                // Если нашли ненулевой элемент в столбце i, меняем строки местами
                if (getElement(j, i) != nullptr && getElement(j, i)->data != 0) {
                    swapRows(i, j);  // Меняем строки i и j местами
                    swapCount++;  // Увеличиваем счетчик перестановок
                    found = true;  // Устанавливаем флаг в true, так как нашли строку для обмена
                    break;
                }
            }
            // Если не удалось найти строку с ненулевым элементом, определитель равен нулю
            if (!found) {
                return 0;  // Возвращаем 0, так как матрица вырождена
            }
            // Обновляем ведущий элемент после обмена строками
            pivotElement = getElement(i, i);
        }

        // Приводим все элементы в столбце i ниже текущего ведущего элемента к нулю
        for (int j = i + 1; j < size; ++j) {
            NODE* targetElement = getElement(j, i);  // Элемент в строке j и столбце i

            if (targetElement != nullptr) {
                int numerator = targetElement->data;  // Элемент, который будем вычитать
                if (numerator != 0) {
                    // Применяем операцию вычитания строки, умноженной на коэффициент
                    additionWithFactor(numerator, j, i);  // Вычитаем строку с нужным коэффициентом
                    // Ведущий элемент найден, умножаем на него масштабирующий коэффициент
                    int pivotValue = pivotElement->data;
                    scalingFactor *= pivotValue;  // Умножаем на ведущий элемент, чтобы учесть его в определителе
                }
            }
        }
    }

    // Вычисляем определитель, умножая все элементы на главной диагонали
    for (int i = 0; i < size; ++i) {
        int diagElement = get(i, i);  // Элемент на главной диагонали
        if (diagElement != 0) {
            det *= diagElement;  // Умножаем на диагональные элементы
        }
        
        else return 0;
    }
    // Если количество перестановок нечетное, инвертируем знак определителя
    if (swapCount % 2 == 1) {
        det = -det;  // Инвертируем знак, так как нечетное количество перестановок изменяет знак определителя
    }

    // Корректируем определитель с учетом масштабирующего коэффициента
    det /= scalingFactor;  // Делаем окончательное деление, чтобы учесть возможные изменения, вызванные вычитанием строк
    std::cout << *this << endl; 
    std::cout << scalingFactor << endl;
    return det;  
}


void SparseMatrix::additionWithFactor(int numerator, int targetRow, int pivotRow) {
    NODE* pivotElement = getElement(pivotRow, pivotRow);  // Указатель на элемент главной строки
    if (!pivotElement) return;  // Проверяем, существует ли элемент
    int coefpivot = pivotElement->data;
    int checkPivot = 0;
/*    NODE* targetElement = getElement(targetRow, pivotElement->col); */ // Элемент в целевой строке с тем же столбцом
    do {
        if (pivotElement) {
            int targetElement = get(pivotElement->row, pivotElement->col);
            int scaledValue = numerator * pivotElement->data;  // Масштабируем значение
            if (targetElement == 0) {
                if (scaledValue != 0) {
                    add(-scaledValue, targetRow, pivotElement->col);  // Добавляем новый элемент
                }
            }
            else {
                targetElement = (targetElement * coefpivot) - scaledValue;  // Обновляем значение
                add(targetElement, targetRow, pivotElement->col);
                if (abs(targetElement) < 1e-10) {
                    add(0, targetRow, pivotElement->col);  // Убираем элемент, если он близок к нулю
                }
            }
            
        }
        pivotElement = pivotElement->nextright;  // Переходим к следующему элементу в главной строке
        if (pivotElement) { 
            checkPivot = pivotElement->data; 
        }
    } while (checkPivot != coefpivot);
}



//void SparseMatrix::additionWithString(int factor, int targetRow, int pivotRow) {
//    NODE* pivotElement = hRow[pivotRow];  
//    while (pivotElement != nullptr) {
//        NODE* targetElement = findElement(targetRow, pivotElement->col);
//
//        if (targetElement == nullptr) {
//            if (factor * pivotElement->data != 0) {
//                addElement(targetRow, pivotElement->col, -factor * pivotElement->data);
//            }
//        }
//        else {
//            targetElement->data -= factor * pivotElement->data;
//            if (abs(targetElement->data) < 1e-10) {
//                removeElement(targetRow, pivotElement->col);
//            }
//        }
//        pivotElement = pivotElement->nextdown;
//    }
//
//    std::cout << "После вычитания строки " << pivotRow << " с множителем " << factor << ":\n";
//    printMatrix();  
//}

SparseMatrix::NODE* SparseMatrix::getElement(int row, int col) const {
    if (row >= size || row < 0 || col >= size || col < 0) {
        return nullptr;
    }
    NODE* current = hRow[row];
    while (current != nullptr) {
        if (current->col == col) {
            return current;
        }
        current = current->nextright;
    }
    return nullptr; 
}

//void SparseMatrix::multiplicationByNumber(int value, int row) {
//    NODE* current = hRow[row];
//    while (current != nullptr) {
//        current->data *= value;
//        current = current->nextright;
//    }
//}

//SparseMatrix::NODE* SparseMatrix::findElement(int targetRow, int col) {
//    if (targetRow >= size || targetRow < 0 || col >= size || col < 0) {
//        return nullptr;
//    }
//
//    NODE* current = hRow[targetRow];
//    while (current != nullptr) {
//        if (current->col == col) {
//            return current;
//        }
//        current = current->nextright;
//    }
//    return nullptr;
//}

void SparseMatrix::addElement(int targetRow, int col, int value) {
    if (targetRow >= size || col >= size) return;
    NODE* newNode = new NODE(value, targetRow, col);

    if (!hRow[targetRow] || hRow[targetRow]->col > col) {
        newNode->nextright = hRow[targetRow];
        hRow[targetRow] = newNode;
    }
    else {
        NODE* current = hRow[targetRow];
        while (current->nextright && current->nextright->col < col) {
            current = current->nextright;
        }
        newNode->nextright = current->nextright;
        current->nextright = newNode;
    }

    if (!hCol[col] || hCol[col]->row > targetRow) {
        newNode->nextdown = hCol[col];
        hCol[col] = newNode;
    }
    else {
        NODE* current = hCol[col];
        while (current->nextdown && current->nextdown->row < targetRow) {
            current = current->nextdown;
        }
        newNode->nextdown = current->nextdown;
        current->nextdown = newNode;
    }
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
            if (rand() % 100 < density) { 
                double value = (rand() % 100) + 1;
                add(value, i, j);
            }
        }
    }
}



ostream& operator<<(ostream& os, const SparseMatrix& matrix) {
    for (int i = 0; i < matrix.size; i++) {
        SparseMatrix::NODE* current = matrix.hRow[i]; 
        for (int j = 0; j < matrix.size; j++) {
            if (current && current->col == j) { 
                os << setw(5) << current->data;
                current = current->nextright; 
            }
            else {
                os << setw(5) << 0; 
            }
        }
        os << endl; 
    }
    return os;
}



