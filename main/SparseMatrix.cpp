#include "SparseMatrix.h"

namespace {
    unsigned greatestCommonDivisor(unsigned a, unsigned b) {
        if (a == 0) {
            return b;
        }
        if (b == 0) {
            return a;
        }
        if (a == b) {
            return a;
        }
        if (a > b) {
            return greatestCommonDivisor(b, a % b);
        }
        return greatestCommonDivisor(a, b % a);
    }
}

SparseMatrix::SparseMatrix()
    : size(0)
    , rows(nullptr)
    , cols(nullptr)
{
}

SparseMatrix::SparseMatrix(unsigned size)
    : size(size)
    , rows(new RowsList[size])
    , cols(new ColsList[size])
{
}

SparseMatrix::~SparseMatrix() {
    clear();
}

void SparseMatrix::clear() {
    if (size) {
        for (unsigned i = 0; i < size; i++) {
            rows[i].reset(true);
            cols[i].reset(false);
        }

        delete[] rows;
        rows = nullptr;

        delete[] cols;
        cols = nullptr;

        size = 0;
    }
}

SparseMatrix::SparseMatrix(SparseMatrix&& other)
    : size(0)
    , rows(nullptr)
    , cols(nullptr)
{
    *this = std::move(other);
}

SparseMatrix& SparseMatrix::operator=(SparseMatrix&& other) {
    if (this != &other) {
        std::swap(size, other.size);
        std::swap(rows, other.rows);
        std::swap(cols, other.cols);
    }
    return *this;
}

void SparseMatrix::set(int data, unsigned row, unsigned col) {
    if (row >= size || col >= size) {
        throw std::out_of_range("Invalid coordinates");
    }

    // если устанавливаем 0 в существующий узел
    if (data == 0) {
        NODE *colNode = rows[row].erase(col);
        NODE *rowNode = cols[col].erase(row);
        assert(colNode == rowNode);
        delete colNode;
    }
    else {
        NODE* newNode = new NODE(data, row, col);
        auto rowInserted = rows[row].insert(newNode);
        if (!rowInserted.first) {
            delete newNode;
            rowInserted.second->data = data;
        }
        else {
            auto colInserted = cols[col].insert(newNode);
            assert(colInserted.first);
        }
    }
}

int SparseMatrix::get(unsigned row, unsigned col) const {
    if (row >= size || col >= size) {
        throw std::out_of_range("Invalid coordinates");
    }

    NODE* node = rows[row].find(col);
    return node ? node->data : 0;
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
                set(value, i, j);
            }
        }
    }
}

SparseMatrix SparseMatrix::operator+(const SparseMatrix& other) const {
    if (size != other.size) {
        throw std::invalid_argument("Matrix sizes do not match");
    }

    SparseMatrix result(size);

    for (unsigned i = 0; i < size; i++) {
        for (auto pair : zip(rows[i], other.rows[i])) {
            if (!pair.first) {
                result.set(pair.second->data, i, pair.second->col);
            }
            else if (!pair.second) {
                result.set(pair.first->data, i, pair.first->col);
            }
            else {
                result.set(pair.first->data + pair.second->data, i, pair.first->col);
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

    for (unsigned i = 0; i < size; i++) {
        for (auto pair : zip(rows[i], other.rows[i])) {
            if (!pair.first) {
                result.set(-pair.second->data, i, pair.second->col);
            }
            else if (!pair.second) {
                result.set(pair.first->data, i, pair.first->col);
            }
            else {
                result.set(pair.first->data - pair.second->data, i, pair.first->col);
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

    for (unsigned i = 0; i < size; i++) {
        for (unsigned j = 0; j < size; j++) {
            int value = 0;
            for (auto pair : zip(rows[i], other.cols[j])) {
                if (pair.first && pair.second) {
                    value += pair.first->data * pair.second->data;
                }
            }
            result.set(value, i, j);
        }
    }

    return result;
}



void SparseMatrix::swapRows(unsigned row1, unsigned row2) {
    if (row1 >= size || row2 >= size || row1 == row2) {
        throw std::out_of_range("Invalid coordinates");
    }

    for (unsigned i = 0; i < size; ++i) {
        int v1 = get(row1, i);
        int v2 = get(row2, i);
        if (v1 != v2) {
            set(v2, row1, i);
            set(v1, row2, i);
        }
    }
}


SparseMatrix SparseMatrix::submatrix(unsigned delRow, unsigned delCol) const {
    if (delRow >= size || delCol >= size) {
        throw std::out_of_range("Invalid coordinates");
    }

    SparseMatrix result(size - 1);

    for (unsigned i = 0; i < size; i++) {
        if (i == delRow) continue;

        for (NODE* node : rows[i]) {
            if (node->col == delCol) continue;

            result.set(node->data, i < delRow ? i : i - 1, node->col < delCol ? node->col : node->col - 1);
        }
    }

    return result;
}

double SparseMatrix::determinant() {
    double det = 1;
    int swapCount = 0;
    int scalingFactor = 1;

    for (unsigned i = 0; i < size; ++i) {
        NODE *pivotElement = cols[i].findEqualOrGreater(i);
        if (!pivotElement) {
            return 0;
        }

        int pivotNumber = pivotElement->data;
        if (pivotElement->row != i) {
            std::cout << "Swap rows " << i << " and " << pivotElement->row << std::endl;
            swapRows(i, pivotElement->row);
            std::cout << *this << std::endl;
            swapCount++;
        }

        for (unsigned j = i + 1; j < size; ++j) {
            NODE *targetElement = cols[i].findEqualOrGreater(j);
            if (!targetElement) {
                break;
            }

            j = targetElement->row;
            int targetNumber = targetElement->data;

            int gcd = greatestCommonDivisor(abs(targetNumber), abs(pivotNumber));
            multiplicationByNumber(pivotNumber / gcd, targetElement->row);
            std::cout << "Multiply row " << j << " by " << pivotNumber / gcd << std::endl;
            std::cout << *this << endl;

            additionWithFactor(-targetNumber / gcd, j, i);
            std::cout << "Add row " << i << " to row " << j << " with factor " << -targetNumber / gcd << std::endl;
            std::cout << *this << endl;
            scalingFactor *= pivotNumber / gcd;  // Умножаем на ведущий элемент, чтобы учесть его в определителе
        }
    }

    for (unsigned i = 0; i < size; ++i) {
        int diagElement = get(i, i);  // Элемент на главной диагонали
        if (diagElement == 0) {
            return 0;
        }

        det *= diagElement;  // Умножаем на диагональные элементы
    }
    if (swapCount % 2 == 1) {
        det = -det;  
    }

    det /= scalingFactor;
    return det;
}

void SparseMatrix::additionWithFactor(int factor, unsigned targetRow, unsigned pivotRow) {
    
    for (NODE *node : rows[pivotRow]) {
    
    
        int oldValue = get(targetRow, node->col);

        set(oldValue + node->data * factor, targetRow, node->col);
    }
}

//SparseMatrix::NODE* SparseMatrix::getElement(int row, int col) const {
//    if (row >= size || row < 0 || col >= size || col < 0) {
//        return nullptr;
//    }
//    NODE* current = rows[row];
//    NODE* start = current;
//    do {
//        if (current->col == col) {
//            return current;
//        }
//        current = current->nextright;
//    } while (current != start);
//    return nullptr; 
//}

void SparseMatrix::multiplicationByNumber(int number, unsigned row) {
    if (number == 0) {
        throw std::invalid_argument("Multiply by zero");
    }
    for (NODE *node : rows[row]) {
        node->data *= number;
    }
}

void SparseMatrix::generateRandomMatrix(unsigned n, int density) {
    *this = SparseMatrix(n);

    srand(static_cast<unsigned>(time(nullptr)));

    for (unsigned i = 0; i < n; i++) {
        for (unsigned j = 0; j < n; j++) {
            if (rand() % 100 < density) { 
                int value = (rand() % 41) - 20; // от -20 до 20
                set(value, i, j);
            }
        }
    }
}

void SparseMatrix::RandomSparseMatrix(unsigned nonZeroElements) {
    unsigned n =  nonZeroElements*10;

    // устанавливаем размер матрицы.
    *this = SparseMatrix(n);

    srand(static_cast<unsigned>(time(nullptr)));

    while (nonZeroElements > 0) {
        unsigned i = rand() % n; // случайная строка.
        unsigned j = rand() % n; // случайный столбец.

        // проверяем, нет ли уже элемента в этой позиции.
        if (!get(i, j)) {
            int value = (rand() % 100) + 1;
            set(value, i, j);
            nonZeroElements--;
        }
    }
}



ostream& operator<<(ostream& os, const SparseMatrix& matrix) {
    for (unsigned i = 0; i < matrix.size; i++) {
        for (unsigned j = 0; j < matrix.size; j++) {
            os << setw(5) << matrix.get(i, j) << ' ';
        }
        os << endl; 
    }
    return os;
}



