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
            if (hRow[i])
            {
                NODE* current = hRow[i];
                NODE* start = current;
                do {
                    NODE* temp = current;
                    current = current->nextright;
                    delete temp;
                } while (current != start);
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
        NODE* start = current;
        if (current) {
            do {
                add(current->data, current->row, current->col);
                current = current->nextright;
            } while (current != start);
        }
    }
}

void SparseMatrix::add(double val, int row, int col) {
    if (row >= size || col >= size || row < 0 || col < 0) {
        throw std::out_of_range("Invalid coordinates");
    }
    // если устанавливаем 0 в существующий узел
    if (val == 0)
    {
        if (hRow[row]) {
            NODE* current = hRow[row];
            NODE* prev = nullptr;

            // Поиск узла в строке
            do {
                if (current->col == col) {
                    // Удаляем узел
                    if (prev) {
                        prev->nextright = current->nextright;
                    }
                    else {
                        // Удаляем первый узел
                        if (current->nextright == current) {
                            hRow[row] = nullptr; // Последний узел в строке
                        }
                        else {
                            hRow[row] = current->nextright;

                            // Находим последний элемент для обновления кольцевой связи
                            NODE* last = hRow[row];
                            while (last->nextright != current) {
                                last = last->nextright;
                            }
                            last->nextright = hRow[row];
                        }
                    }
                    break;
                }
                prev = current;
                current = current->nextright;
            } while (current != hRow[row]);
        }

        // Работа с hCol[col]
        if (hCol[col]) {
            NODE* current = hCol[col];
            NODE* prev = nullptr;

            // Поиск узла в столбце
            do {
                if (current->row == row) {
                    // Удаляем узел
                    if (prev) {
                        prev->nextdown = current->nextdown;
                    }
                    else {
                        // Удаляем первый узел
                        if (current->nextdown == current) {
                            hCol[col] = nullptr; // Последний узел в столбце
                        }
                        else {
                            hCol[col] = current->nextdown;

                            // Находим последний элемент для обновления кольцевой связи
                            NODE* last = hCol[col];
                            while (last->nextdown != current) {
                                last = last->nextdown;
                            }
                            last->nextdown = hCol[col];
                        }
                    }
                    break;
                }
                prev = current;
                current = current->nextdown;
            } while (current != hCol[col]);
        }
    }
    // Работа с hRow
    NODE* newNode = new NODE(val, row, col);
    if (!hRow[row]) {
        // Если строка пуста
        hRow[row] = newNode;
        newNode->nextright = newNode; // Указывает на самого себя
    }
    else {
        NODE* current = hRow[row];
        NODE* prev = nullptr;
        // Обходим список с учетом кольцевой структуры
        do {
            if (current->col >= col) break; // Найдено место для вставки
            prev = current;
            current = current->nextright;
        } while (current != hRow[row]);
        if (prev) {
            // Вставка между prev и current
            prev->nextright = newNode;
            newNode->nextright = current;
            // Если вставляем перед первым элементом, обновляем hRow[row]
            if (current == hRow[row] && newNode->col < hRow[row]->col) {
                hRow[row] = newNode;
            }
        }
        else {
            // Вставка перед первым элементом
            newNode->nextright = hRow[row];
            // Найти последний элемент для обновления кольцевой связи
            NODE* last = hRow[row];
            while (last->nextright != hRow[row]) {
                last = last->nextright;
            }
            last->nextright = newNode;
            hRow[row] = newNode; // Новый узел становится началом
        }
    }
    // Работа с hCol
    if (!hCol[col]) {
        // Если колонка пуста
        hCol[col] = newNode;
        newNode->nextdown = newNode; // Указывает на самого себя
    }
    else {
        NODE* current = hCol[col];
        NODE* prev = nullptr;

        // Обходим список с учетом кольцевой структуры
        do {
            if (current->row >= row) break; // Найдено место для вставки
            prev = current;
            current = current->nextdown;
        } while (current != hCol[col]);

        if (prev) {
            // Вставка между prev и current
            prev->nextdown = newNode;
            newNode->nextdown = current;

            // Если вставляем перед первым элементом, обновляем hCol[col]
            if (current == hCol[col] && newNode->row < hCol[col]->row) {
                hCol[col] = newNode;
            }
        }
        else {
            // Вставка перед первым элементом
            newNode->nextdown = hCol[col];

            // Найти последний элемент для обновления кольцевой связи
            NODE* last = hCol[col];
            while (last->nextdown != hCol[col]) {
                last = last->nextdown;
            }
            last->nextdown = newNode;

            hCol[col] = newNode; // Новый узел становится началом
        }
    }

}

int SparseMatrix::get(int row, int col) const {
    if (row >= size || col >= size || row < 0 || col < 0) {
        cout << "Ошибка в координатах" << endl;
        return 0;
    }

    NODE* current = hRow[row];
    if (!current) return 0;
    NODE* start = current;
    do {
        if (current->col == col) {
            return current->data;  
        }
        current = current->nextright;
    } while (start != current);
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
    std::swap(hRow[row1], hRow[row2]);

    for (int i = 0; i < size; ++i) {
        NODE* current = hCol[i];
        if (current)
        {
            NODE* start = current;
            do {
                if (current->row == row1) {
                    current->row = row2;
                }
                else if (current->row == row2) {
                    current->row = row1;
                }
                current = current->nextdown;

            } while (start != current);
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
    SparseMatrix A = *this;
    double det = 1;  
    int swapCount = 0; 
    int scalingFactor = 1; 

    for (int i = 0; i < size; ++i) {
        if (!hRow[i] || !hCol[i]) return 0;
        NODE* pivotElement = A.getElement(i, i); 
        if (pivotElement == nullptr) {
            bool found = false;  
            for (int j = i + 1; j < size; ++j) {
                if (A.getElement(j, i) != nullptr) {
                    A.swapRows(i, j); 
                    swapCount++;  
                    found = true;  
                    break;
                }
            }
            if (!found) {
                return 0; 
            }
            pivotElement = A.getElement(i, i);
        }
        for (int j = i + 1; j < size; ++j) {
            NODE* targetElement = A.getElement(j, i);  // Элемент в строке j и столбце i
            int pivotNumber = pivotElement->data;
            if (targetElement != nullptr) {
                int numerator = targetElement->data;  // Элемент, из которого будем вычитать
                if (numerator != 0) {
                    A.multiplicationByNumber(pivotNumber, j);
                    A.additionWithFactor(numerator, j, i);  
                    int pivotValue = pivotElement->data;
                    scalingFactor *= pivotValue;  // Умножаем на ведущий элемент, чтобы учесть его в определителе
                }
            }
        }
    }
    for (int i = 0; i < size; ++i) {
        int diagElement = A.get(i, i);  // Элемент на главной диагонали
        if (diagElement != 0) {
            det *= diagElement;  // Умножаем на диагональные элементы
        }
        
        else return 0;
    }
    if (swapCount % 2 == 1) {
        det = -det;  
    }

    det /= scalingFactor;  
      return det;  
}

void SparseMatrix::additionWithFactor(int numerator, int targetRow, int pivotRow) {
    
    NODE* pivotElement = getElement(pivotRow, pivotRow);  
    
    if (!pivotElement) return; 
    NODE* checkPivot = getElement(pivotRow, pivotRow);
    
    do {
            int targetElement = get(targetRow, pivotElement->col);

            int scaledValue = numerator * pivotElement->data;  // Масштабируем значение
            if (targetElement == 0) {
                if (scaledValue != 0) {
                    add(-scaledValue, targetRow, pivotElement->col);  // Добавляем новый элемент
                }
            }
            else {
                targetElement -=  scaledValue;  // Обновляем значение
                add(targetElement, targetRow, pivotElement->col);
                
            }
        pivotElement = pivotElement->nextright;  // Переходим к следующему элементу в главной строке 
    } while (checkPivot != pivotElement);
}

SparseMatrix::NODE* SparseMatrix::getElement(int row, int col) const {
    if (row >= size || row < 0 || col >= size || col < 0) {
        return nullptr;
    }
    NODE* current = hRow[row];
    if (current)
    {
        NODE* start = current;
        do {
            if (current->col == col) {
                return current;
            }
            current = current->nextright;
        } while (current != start);
    }
    return nullptr; 
}

void SparseMatrix::multiplicationByNumber(int value, int row) {
    NODE* current = hRow[row];
    NODE* start = hRow[row];
    do {
        current->data *= value;
        current = current->nextright;
    } while (current != start);
}

void SparseMatrix::generateRandomMatrix(int n, int density) {
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
                int value = (rand() % 100) + 1;
                add(value, i, j);
            }
        }
    }
}

void SparseMatrix::RandomSparseMatrix(int nonZeroElements) {
    int n =  nonZeroElements*10;

    // Устанавливаем размер матрицы.
    size = n;
    hRow = new NODE * [n];
    hCol = new NODE * [n];
    for (size_t i = 0; i < n; i++) {
        hRow[i] = nullptr;
        hCol[i] = nullptr;
    }

    srand(time(0));

    while (nonZeroElements > 0) {
        size_t i = rand() % n; // Случайная строка.
        size_t j = rand() % n; // Случайный столбец.

        // Проверяем, нет ли уже элемента в этой позиции.
        if (!get(i, j)) {
            int value = (rand() % 100) + 1;
            add(value, i, j);
            nonZeroElements--;
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



