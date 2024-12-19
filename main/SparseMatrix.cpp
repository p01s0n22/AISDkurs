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
    if (!hRow || !hCol) return;

    for (size_t i = 0; i < size; i++) {
        if (hRow[i]) {
            NODE* current = hRow[i];
            do {
                NODE* toDelete = current;
                current = current->nextright;
                delete toDelete; 
            } while (current && current != hRow[i]);
            hRow[i] = nullptr;
        }
    }

    delete[] hRow;
    delete[] hCol;
    hRow = nullptr;
    hCol = nullptr;
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
    if (other.size == 0) return;

    for (size_t i = 0; i < other.size; i++) {
        NODE* current = other.hRow[i];
        if (!current) continue;

        do {
            add(current->data, current->row, current->col); // Создаём новые узлы
            current = current->nextright;
        } while (current != other.hRow[i]);
    }
}

void SparseMatrix::add(double val, int row, int col) {
    if (val == 0) return;

    if (row >= size || col >= size || row < 0 || col < 0) {
        throw std::out_of_range("Неверные координаты!");
    }

    // Удаляем существующий узел, если он уже есть
    NODE* current = hRow[row];
    if (current) {
        NODE* prev = nullptr;
        do {
            if (current->col == col) {
                current->data = val; // Обновляем значение
                return;
            }
            prev = current;
            current = current->nextright;
        } while (current && current != hRow[row]);
    }

    NODE* newNode = new NODE(val, row, col); // Создаём новый узел

    // Вставка в строку
    if (!hRow[row]) {
        hRow[row] = newNode;
        newNode->nextright = newNode; // Создаём кольцевой список
    }
    else {
        NODE* current = hRow[row];
        NODE* prev = nullptr;
        do {
            if (current->col > col) break;
            prev = current;
            current = current->nextright;
        } while (current && current != hRow[row]);

        if (!prev) {
            NODE* last = hRow[row];
            while (last->nextright != hRow[row]) last = last->nextright;

            newNode->nextright = hRow[row];
            last->nextright = newNode;
            hRow[row] = newNode;
        }
        else {
            prev->nextright = newNode;
            newNode->nextright = current;
        }
    }

    // Вставка в столбец
    if (!hCol[col]) {
        hCol[col] = newNode;
        newNode->nextdown = newNode;
    }
    else {
        NODE* current = hCol[col];
        NODE* prev = nullptr;
        do {
            if (current->row > row) break;
            prev = current;
            current = current->nextdown;
        } while (current && current != hCol[col]);

        if (!prev) {
            NODE* last = hCol[col];
            while (last->nextdown != hCol[col]) last = last->nextdown;

            newNode->nextdown = hCol[col];
            last->nextdown = newNode;
            hCol[col] = newNode;
        }
        else {
            prev->nextdown = newNode;
            newNode->nextdown = current;
        }
    }
}


double SparseMatrix::get(int row, int col) const {
    if (row >= size || col >= size || row < 0 || col < 0) {
        cout << "Ошибка в координатах" << endl;
        return 0;
    }

    NODE* current = hRow[row];
    if (!current) return 0;

    do {
        if (current->col == col) {
            return current->data;
        }
        current = current->nextright;
    } while (current != hRow[row]);

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

void SparseMatrix::inputFromFile(const char* filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Ошибка открытия файла!" << endl;
        return;
    }

    int n;
    file >> n;
    if (n <= 0) {
        cout << "Размер матрицы должен быть положительным!" << endl;
        return;
    }
    *this = SparseMatrix(n);
    cout << "Считывание матрицы из файла..." << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int value;
            file >> value;
            if (value != 0) {
                add(value, i, j);
            }
        }
    }

    file.close();
    cout << "Матрица успешно считана." << endl;
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
    if (size == 0) return 0;
    if (size == 1) return get(0, 0);

    SparseMatrix A(*this); // Копируем текущую матрицу
    double sign = 1.0;

    std::cout << "Начальная матрица:\n";
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            std::cout << A.get(i, j) << " ";
        }
        std::cout << "\n";
    }

    // Основной цикл
    for (size_t i = 0; i < size; i++) {
        // Поиск строки с максимальным элементом в текущем столбце
        int maxRow = i;
        for (size_t r = i + 1; r < size; r++) {
            if (std::abs(A.get(r, i)) > std::abs(A.get(maxRow, i))) {
                maxRow = r;
            }
        }

        // Если в текущем столбце все элементы равны 0, определитель равен 0
        if (A.get(maxRow, i) == 0.0) {
            std::cout << "Столбец " << i << " состоит из нулей, определитель = 0.\n";
            return 0.0;
        }

        // Перестановка строк
        if (maxRow != i) {
            A.swapRows(maxRow, i);
            sign *= -1.0; // Перестановка строк меняет знак определителя
            std::cout << "Поменяли местами строки " << i << " и " << maxRow << ":\n";
        }

        // Прямой ход метода Гаусса
        for (size_t j = i + 1; j < size; j++) {
            if (A.get(j, i) != 0.0) {
                double factor = A.get(j, i) / A.get(i, i);
                std::cout << "Обнуляем элемент A(" << j << ", " << i << "), коэффициент: " << factor << "\n";
                for (size_t k = 0; k < size; k++) {
                    double newValue = A.get(j, k) - factor * A.get(i, k);
                    if (std::abs(newValue) < 1e-9) newValue = 0.0;
                    A.add(newValue, j, k);  // Предполагается, что есть метод set
                }
            }
        }

        // Печать состояния матрицы после завершения обработки текущего столбца
        std::cout << "Матрица после обработки столбца " << i << ":\n";
        for (size_t i = 0; i < size; ++i) {
            for (size_t j = 0; j < size; ++j) {
                std::cout << A.get(i, j) << " ";
            }
            std::cout << "\n";
        }
    }

    // Вычисляем произведение диагональных элементов (верхнетреугольной матрицы)
    double det = sign;
    std::cout << "Вычисление определителя, умножаем элементы на диагонали:\n";
    for (size_t i = 0; i < size; i++) {
        double diagonalValue = A.get(i, i);
        if (std::abs(diagonalValue) < 1e-9) {
            std::cout << "На диагонали ноль, определитель = 0.\n";
            return 0.0;
        }
        std::cout << "A(" << i << ", " << i << ") = " << diagonalValue << "\n";
        det *= diagonalValue;
    }

    std::cout << "Итоговая верхнетреугольная матрица:\n";
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            std::cout << A.get(i, j) << " ";
        }
        std::cout << "\n";
    }

    std::cout << "Определитель = " << det << "\n";
    return det;
}




void SparseMatrix::generateRandomMatrix(size_t n, int density) {
    if (density < 1 || density > 100) {
        throw std::invalid_argument("Плотность должна быть в диапазоне от 1 до 100.");
    }
    *this = SparseMatrix(n); 
    srand(static_cast<unsigned>(time(nullptr)));

    int totalElements = n * n * density / 100; 

    for (int i = 0; i < totalElements; i++) {
        int row = rand() % n;      
        int col = rand() % n;            
        int value = rand() % 100 + 1;   
        if (get(row, col) == 0) {
            add(value, row, col);
        }
        else {
            i--; 
        }
    }
}

ostream& operator<<(ostream& os, const SparseMatrix& matrix) {
    for (size_t i = 0; i < matrix.size; i++) {
        SparseMatrix::NODE* current = matrix.hRow[i];
        for (size_t j = 0; j < matrix.size; j++) {
            bool found = false;

            if (current) {
                do {
                    if (current->col == j) {
                        os << setw(3) << current->data << " ";
                        found = true;
                        current = current->nextright;
                        break;
                    }
                    current = current->nextright; 
                } while (current != matrix.hRow[i]);
            }

            if (!found) {
                os << setw(3) << 0 << " "; 
            }
        }
        os << endl;
    }
    return os;
}
