#include "SparseMatrix.h"

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

double SparseMatrix::determinant() {
    double det = 1;
    int swapCount = 0; // Счётчик перестановок строк

    // Прямой ход (приведение матрицы к верхнетреугольному виду)
    for (int i = 0; i < size; ++i) {
        NODE* pivotElement = hRow[i];

        // Если пивот равен 0, ищем ненулевой элемент в столбце и меняем строки
        if (pivotElement->data == 0) {
            bool found = false;
            for (int j = i + 1; j < size; ++j) {
                if (hRow[j]->data != 0) {
                    swapRows(i, j); // Меняем строки
                    swapCount++;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return 0; // Если не нашли ненулевого пивота, определитель равен 0
            }
        }

        // Приводим строки ниже пивота к верхнетреугольному виду
        for (int j = i + 1; j < size; ++j) {
            double factor = hRow[j]->data / pivotElement->data;
            additionWithString(factor, j, i); // Вычитаем строку с множителем
        }
    }

    // Теперь вычисляем определитель, умножая элементы главной диагонали
    for (int i = 0; i < size; ++i) {
        NODE* diagElement = hRow[i];
        det *= diagElement->data;
    }

    // Если было нечетное количество перестановок строк, меняем знак определителя
    if (swapCount % 2 != 0) {
        det = -det;
    }

    return det;
}




void SparseMatrix::additionWithString(double factor, int targetRow, int pivotRow) {
    NODE* pivotElement = hRow[pivotRow];  // Используем hRow для доступа к строкам
    while (pivotElement != nullptr) {
        NODE* targetElement = findElement(targetRow, pivotElement->col);

        if (targetElement == nullptr) {
            // Если элемент не найден в целевой строке, добавляем его с нужным значением
            if (factor * pivotElement->data != 0) {
                addElement(targetRow, pivotElement->col, -factor * pivotElement->data);
            }
        }
        else {
            // Если элемент найден, вычитаем его из целевого элемента
            targetElement->data -= factor * pivotElement->data;
            // Если результат очень мал, удаляем элемент
            if (abs(targetElement->data) < 1e-10) {
                removeElement(targetRow, pivotElement->col);
            }
        }
        pivotElement = pivotElement->nextdown; // Переходим к следующему элементу в строке
    }

    // Отладочный вывод после вычитания
    std::cout << "После вычитания строки " << pivotRow << " с множителем " << factor << ":\n";
    printMatrix();  // Выводим обновленную матрицу для отладки
}

SparseMatrix::NODE* SparseMatrix::getElement(int row, int col) const {
    NODE* current = hRow[row];
    while (current != nullptr) {
        if (current->col == col) {
            return current;
        }
        current = current->nextright;
    }
    return nullptr; // Если элемента нет
}

void SparseMatrix::multiplicationByNumber(double value, int row) {
    NODE* current = hRow[row];
    while (current != nullptr) {
        current->data *= value;
        current = current->nextright;
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


SparseMatrix::NODE* SparseMatrix::findElement(int targetRow, int col) {
    if (targetRow >= size || targetRow < 0 || col >= size || col < 0) {
        return nullptr; // Проверка на выход за границы
    }

    NODE* current = hRow[targetRow]; // Начало строки
    while (current != nullptr) {
        if (current->col == col) {
            return current; // Найден элемент
        }
        current = current->nextright; // Переход к следующему элементу строки
    }
    return nullptr; // Элемент не найден
}

void SparseMatrix::addElement(int targetRow, int col, double value) {
    if (targetRow >= size || col >= size) return;
    NODE* newNode = new NODE(value, targetRow, col);

    // Добавление в строку
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

    // Добавление в столбец
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

void SparseMatrix::removeElement(int targetRow, int col) {
    if (targetRow >= size || col >= size) return;

    // Удаление из строки
    NODE* prev = nullptr;
    NODE* current = hRow[targetRow];
    while (current && current->col != col) {
        prev = current;
        current = current->nextright;
    }
    if (current) {
        if (prev) {
            prev->nextright = current->nextright;
        }
        else {
            hRow[targetRow] = current->nextright;
        }
    }

    // Удаление из столбца
    prev = nullptr;
    current = hCol[col];
    while (current && current->row != targetRow) {
        prev = current;
        current = current->nextdown;
    }
    if (current) {
        if (prev) {
            prev->nextdown = current->nextdown;
        }
        else {
            hCol[col] = current->nextdown;
        }
        delete current;
    }
}

void SparseMatrix::printMatrix() {
    for (int i = 0; i < size; ++i) {
        NODE* current = hRow[i];
        cout << "Row " << i << ": ";
        while (current) {
            cout << "(" << current->row << "," << current->col << "," << current->data << ") ";
            current = current->nextright;
        }
        cout << endl;
    }
}