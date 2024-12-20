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

void SparseMatrix::add(int val, int row, int col) {
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



int SparseMatrix::get(int row, int col) const {
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

SparseMatrix::NODE* SparseMatrix::getElement(int row, int col) const {
    if (row < 0 || col < 0 || row >= size || col >= size) {
        throw std::out_of_range("Invalid row or column index");
    }

    // Поиск нужного элемента в строке
    NODE* current = hRow[row];
    while (current != nullptr) {
        if (current->col == col) {
            return current;  // Возвращаем указатель на элемент
        }
        current = current->nextright;
    }

    return nullptr;  // Если элемент не найден, возвращаем nullptr
}

double SparseMatrix::determinant() {
    if (size == 0) return 1; // Пустая матрица
    double det = 1;          // Начальное значение определителя
    int sign = 1;            // Знак определителя, меняется при перестановках строк

    int j = 0; // Номер текущего столбца
    for (int i = 0; i < size - 1; ++i) {
        NODE* firstElement = getElement(i, j); // Ведущий элемент
        while (firstElement == nullptr) {
            bool swapped = false;

            // Поиск строки для перестановки
            for (int next_i = i + 1; next_i < size; ++next_i) {
                if (getElement(next_i, j) != nullptr) {
                    swapRows(i, next_i); // Перестановка строк
                    sign *= -1;              // Изменяем знак определителя
                    swapped = true;
                    break;
                }
            }

            if (!swapped) {
                ++j; // Если столбец пустой, переходим к следующему
                if (j >= size) return 0; // Матрица вырождена
                firstElement = getElement(i, j);
            }
            else {
                firstElement = getElement(i, j);
            }
        }

        // Приведение нижележащих строк
        for (int r = i + 1; r < size; ++r) {
            NODE* temp = getElement(r, j);
            if (temp == nullptr) continue; // Пропускаем, если элемент нулевой

            // Вычисляем коэффициент для обнуления
            double coeff = -temp->value / firstElement->value;
            additionWithString(coeff, r, i); // Вычитаем строку i из строки r
        }

        ++j; // Переходим к следующему столбцу
    }

    // Умножаем диагональные элементы
    for (int i = 0; i < size; ++i) {
        NODE* diag = getElement(i, i);
        if (diag == nullptr) return 0; // Нулевой элемент на диагонали => определитель равен 0
        det *= diag->value;
    }

    return det * sign; // Возвращаем определитель с учетом перестановок
}



void SparseMatrix::additionWithString(int factor, int rowDest, int rowSource) {
    NODE* dest = hRow[rowDest];
    NODE* source = hRow[rowSource];

    while (dest != nullptr && source != nullptr) {
        if (dest->col == source->col) {
            dest->data += factor * source->data;  // Вычитание с коэффициентом
            dest = dest->nextright;
            source = source->nextright;
        }
        else if (dest->col < source->col) {
            dest = dest->nextright;
        }
        else {
            source = source->nextright;
        }
    }
}

void SparseMatrix::multiplicationByNumber(int factor, int row) {
    NODE* current = hRow[row];
    while (current != nullptr) {
        current->data *= factor;  // Умножаем значение на коэффициент
        current = current->nextright;
    }
}


void SparseMatrix::generateRandomMatrix(size_t n, int density) {
    if (density < 1 || density > 100) {
        throw std::invalid_argument("Плотность должна быть в диапазоне от 1 до 100.");
    }

    *this = SparseMatrix(n);
    srand(static_cast<unsigned>(time(nullptr)));

    int totalElements = n * n * density / 100;

    bool** used = new bool* [n];
    for (size_t i = 0; i < n; i++) {
        used[i] = new bool[n]();
    }

    for (int i = 0; i < totalElements;) {
        int row = rand() % n;
        int col = rand() % n;

        if (!used[row][col]) {
            int value = rand() % 100 + 1;
            add(value, row, col);
            used[row][col] = true;
            i++;
            std::cout << "Добавлено: " << value << " в позицию (" << row << ", " << col << ")\n";
        }
    }

    for (size_t i = 0; i < n; i++) {
        delete[] used[i];
    }
    delete[] used;
}




ostream& operator<<(ostream& os, const SparseMatrix& matrix) {
    for (size_t i = 0; i < matrix.size; i++) {
        SparseMatrix::NODE* current = matrix.hRow[i];
        for (size_t j = 0; j < matrix.size; j++) {
            if (current && current->col == j) {
                os << setw(3) << current->data << " ";
                current = current->nextright; // Переход к следующему элементу
            }
            else {
                os << setw(3) << 0 << " ";
            }
        }
        os << std::endl;
    }
    return os;
}

