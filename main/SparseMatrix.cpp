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

//SparseMatrix::~SparseMatrix() {
//    if (hRow) {
//        for (int i = 0; i < size; i++) {
//            NODE* current = hRow[i];
//            if (current) {
//                // Удаляем узлы в строке
//                NODE* temp = nullptr;
//                do {
//                    temp = current->nextright;  // Сохраняем ссылку на следующий элемент
//                    delete current;             // Удаляем текущий узел
//                    current = temp;             // Переходим к следующему узлу
//                } while (current != hRow[i]);  // Выход из цикла при достижении начала строки
//            }
//        }
//    }
//
//    if (hCol) {
//        for (int i = 0; i < size; i++) {
//            NODE* current = hCol[i];
//            if (current) {
//                // Удаляем узлы в столбце
//                NODE* temp = nullptr;
//                do {
//                    temp = current->nextdown;  // Сохраняем ссылку на следующий элемент
//                    delete current;            // Удаляем текущий узел
//                    current = temp;            // Переходим к следующему узлу
//                } while (current != hCol[i]);  // Выход из цикла при достижении начала столбца
//            }
//        }
//    }
//
//    delete[] hRow;
//    delete[] hCol;
//
//    hRow = nullptr;
//    hCol = nullptr;
//    size = 0;
//}

void SparseMatrix::add(int val, int row, int col) {
    if (val == 0) return; 

    if (row >= size || col >= size || row < 0 || col < 0) {
        throw std::out_of_range("Неверные координаты!");
    }
    NODE* newNode = new NODE(val, row, col);
    if (!hRow[row]) { 
        hRow[row] = newNode;
        newNode->nextright = newNode; 
    }
    else {
        NODE* current = hRow[row];
        NODE* prev = nullptr;

        do {
            if (current->col == col) {
                current->data = val;  
                delete newNode;       
                return;
            }
            if (current->col > col) break; 
            prev = current;
            current = current->nextright;
        } while (current != hRow[row]);

        if (prev == nullptr) { 
            newNode->nextright = current;
            NODE* last = hRow[row];
            while (last->nextright != hRow[row]) {
                last = last->nextright;
            }
            last->nextright = newNode;
            hRow[row] = newNode; 
        }
        else {
            prev->nextright = newNode;
            newNode->nextright = current;
        }
    }
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
        } while (current != hCol[col]);

        if (prev == nullptr) { 
            newNode->nextdown = current;
            NODE* last = hCol[col];
            while (last->nextdown != hCol[col]) {
                last = last->nextdown;
            }
            last->nextdown = newNode;
            hCol[col] = newNode; 
        }
        else { 
            prev->nextdown = newNode;
            newNode->nextdown = current;
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
    if (this->size != other.size) {
        throw std::invalid_argument("Матрицы разных размеров!");
    }

    SparseMatrix result(this->size);
    for (int i = 0; i < this->size; i++) {
        SparseMatrix::NODE* currA = this->hRow[i];
        SparseMatrix::NODE* currB = other.hRow[i];

        while (currA || currB) {
            int colA = currA ? currA->col : INT_MAX;
            int colB = currB ? currB->col : INT_MAX;

            if (colA == colB) {
                result.add(currA->data + currB->data, i, colA);
                currA = currA->nextright;
                currB = currB->nextright;
            }
            else if (colA < colB) {
                result.add(currA->data, i, colA);
                currA = currA->nextright;
            }
            else {
                result.add(currB->data, i, colB);
                currB = currB->nextright;
            }
        }
    }
    return result;
}

SparseMatrix SparseMatrix::operator-(const SparseMatrix& other) const {
    if (this->size != other.size) {
        throw std::invalid_argument("Матрицы разных размеров!");
    }

    SparseMatrix result(this->size);
    for (int i = 0; i < this->size; i++) {
        SparseMatrix::NODE* currA = this->hRow[i];
        SparseMatrix::NODE* currB = other.hRow[i];

        while (currA || currB) {
            int colA = currA ? currA->col : INT_MAX;
            int colB = currB ? currB->col : INT_MAX;

            if (colA == colB) {
                result.add(currA->data - currB->data, i, colA);
                currA = currA->nextright;
                currB = currB->nextright;
            }
            else if (colA < colB) {
                result.add(currA->data, i, colA);
                currA = currA->nextright;
            }
            else {
                result.add(-currB->data, i, colB);
                currB = currB->nextright;
            }
        }
    }
    return result;
}

SparseMatrix SparseMatrix::operator*(const SparseMatrix& other) const {
    if (this->size != other.size) {
        throw std::invalid_argument("Матрицы разных размеров!");
    }

    SparseMatrix result(this->size);

    for (int i = 0; i < this->size; i++) {
        SparseMatrix::NODE* currA = this->hRow[i];

        while (currA) {
            SparseMatrix::NODE* currB = other.hCol[currA->col];
            while (currB) {
                int row = i;
                int col = currB->col;
                int value = currA->data * currB->data;
                int existingValue = result.get(row, col);
                result.add(existingValue + value, row, col);

                currB = currB->nextdown;
            }
            currA = currA->nextright;
        }
    }

    return result;
}

void SparseMatrix::swapRows(int row1, int row2) {
    if (row1 < 0 || row2 < 0 || row1 >= size || row2 >= size) {
        throw std::out_of_range("Неверный номер строки!");
    }
    std::swap(hRow[row1], hRow[row2]);
}

SparseMatrix SparseMatrix::submatrix(int delRow, int delCol) const {
    SparseMatrix result(this->size - 1);

    for (int i = 0, newRow = 0; i < this->size; i++) {
        if (i == delRow) continue;

        SparseMatrix::NODE* curr = this->hRow[i];
        while (curr) {
            if (curr->col != delCol) {
                result.add(curr->data, newRow, curr->col > delCol ? curr->col - 1 : curr->col);
            }
            curr = curr->nextright;
        }
        newRow++;
    }
    return result;
}

int SparseMatrix::determinant() const {
    if (size == 0) {
        throw std::logic_error("Матрица пуста!");
    }
    SparseMatrix copy(*this); 
    int determinant = 1;
    int swaps = 0; 

    for (int col = 0; col < size; col++) {
        int pivotRow = -1;
        for (int row = col; row < size; row++) {
            if (copy.get(row, col) != 0) {
                pivotRow = row;
                break;
            }
        }
        if (pivotRow == -1) return 0;
        if (pivotRow != col) {
            copy.swapRows(pivotRow, col);
            swaps++;
        }
        int pivot = copy.get(col, col);
        if (pivot == 0) return 0; 
        determinant *= pivot;
        for (int row = col + 1; row < size; row++) {
            int factor = copy.get(row, col);
            if (factor != 0) {
                for (int i = col; i < size; i++) {
                    int value = copy.get(row, i) - factor * copy.get(col, i) / pivot;
                    copy.add(value, row, i);
                }
            }
        }
    }
    if (swaps % 2 != 0) determinant = -determinant;
    return determinant;
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
