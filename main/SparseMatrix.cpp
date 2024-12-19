
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
    do {
        if (current->col == col) {
            return current->data;  // Найден элемент в нужной ячейке
        }
        current = current->nextright;
    } while (current != nullptr);  // Если дошли до конца списка

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
        throw std::invalid_argument("Размеры матриц должны совпадать!");
    }

    SparseMatrix result(size);

    for (size_t i = 0; i < size; i++) {
        NODE* currentA = hRow[i];
        NODE* currentB = other.hRow[i];

        while (currentA || currentB) {
            if (currentA && (!currentB || currentA->col < currentB->col)) {
                // Если элемент есть только в первой матрице
                result.add(currentA->data, currentA->row, currentA->col);
                currentA = currentA->nextright;
            }
            else if (currentB && (!currentA || currentB->col < currentA->col)) {
                // Если элемент есть только во второй матрице
                result.add(currentB->data, currentB->row, currentB->col);
                currentB = currentB->nextright;
            }
            else {
                int sum = currentA->data + currentB->data;
                if (sum != 0) {
                    result.add(sum, currentA->row, currentA->col);
                }
                currentA = currentA->nextright;
                currentB = currentB->nextright;
            }
        }
    }

    return result;
}


SparseMatrix SparseMatrix::operator-(const SparseMatrix& other) const {
    if (size != other.size) {
        throw std::invalid_argument("Размеры матриц должны совпадать!");
    }

    SparseMatrix result(size);
    for (size_t i = 0; i < size; i++) {
        NODE* currentA = hRow[i];
        NODE* currentB = other.hRow[i];

        while (currentA || currentB) {
            if (currentA && (!currentB || currentA->col < currentB->col)) {
                result.add(currentA->data, currentA->row, currentA->col);
                currentA = currentA->nextright;
            }
            else if (currentB && (!currentA || currentB->col < currentA->col)) {
                result.add(-currentB->data, currentB->row, currentB->col);
                currentB = currentB->nextright;
            }
            else {
                int diff = currentA->data - currentB->data;
                if (diff != 0) {
                    result.add(diff, currentA->row, currentA->col);
                }
                currentA = currentA->nextright;
                currentB = currentB->nextright;
            }
        }
    }

    return result;
}


SparseMatrix SparseMatrix::operator*(const SparseMatrix& other) const {
    if (size != other.size) {
        throw std::invalid_argument("Размеры матриц должны совпадать!");
    }

    SparseMatrix result(size);
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            double sum = 0;
            NODE* currentA = hRow[i];
            while (currentA) {
                NODE* currentB = other.hCol[j];
                while (currentB) {
                    if (currentA->col == currentB->row) {
                        sum += currentA->data * currentB->data;
                    }
                    currentB = currentB->nextdown;
                }
                currentA = currentA->nextright;
            }
            if (sum != 0) {
                result.add(sum, i, j);
            }
        }
    }

    return result;
}
void SparseMatrix::swapRows(int row1, int row2) {
    if (row1 < 0 || row1 >= size || row2 < 0 || row2 >= size) {
        throw std::out_of_range("Неверные индексы строк!");
    }
    std::swap(hRow[row1], hRow[row2]);
    NODE* current = hRow[row1];
    if (current) {
        NODE* firstNode = current;
        do {
            current->row = row1;
            current = current->nextright;
        } while (current != firstNode);
    }
    current = hRow[row2];
    if (current) {
        NODE* firstNode = current;
        do {
            current->row = row2;
            current = current->nextright;
        } while (current != firstNode);
    }
}




SparseMatrix SparseMatrix::submatrix(int delRow, int delCol) const {
    if (delRow < 0 || delRow >= size || delCol < 0 || delCol >= size) {
        throw std::out_of_range("Неверные координаты для подматрицы!");
    }

    SparseMatrix result(size - 1);

    for (size_t i = 0, newRow = 0; i < size; i++) {
        if (i == delRow) continue;

        for (size_t j = 0, newCol = 0; j < size; j++) {
            if (j == delCol) continue;

            double value = get(i, j);
            if (value != 0) {
                result.add(value, newRow, newCol);
            }
            if (j != delCol) newCol++;
        }
        if (i != delRow) newRow++;
    }

    return result;
}

double SparseMatrix::determinant() const {
    if (size == 0)
        return 0;
    if (size == 1)
        return hRow[0] && hRow[0]->col == 0 ? hRow[0]->data : 0;

    double det = 0;
    for (int i = 0; i < size; i++) {
        if (hRow[0] == nullptr)
            continue;

        NODE* current = hRow[0];
        while (current != nullptr && current->col != i)
            current = current->nextright;

        if (current != nullptr) {
            SparseMatrix sub = submatrix(0, i);
            double cofactor = current->data * ((i % 2 == 0) ? 1 : -1);
            det += cofactor * sub.determinant();
        }
    }
    return det;
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