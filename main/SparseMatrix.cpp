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
}


SparseMatrix::~SparseMatrix() {
    for (int i = 0; i < size; ++i) {
        if (hRow[i]) {
            NODE* curr = hRow[i];
            NODE* nextNode = nullptr;

            // Если в строке только один элемент
            if (curr->nextright == curr) {
                // Удаляем единственный элемент в строке
                hRow[i] = nullptr; // Строка пустая
                delete curr;
            }
            else {
                // Если в строке несколько элементов
                do {
                    nextNode = curr->nextright; // Переход к следующему элементу
                    // Убираем связи по столбцу
                    if (curr->nextdown) {
                        NODE* colCurr = hCol[curr->col];
                        NODE* colPrev = nullptr;

                        // Убираем связь в столбце
                        while (colCurr != nullptr && colCurr != hCol[curr->col]) {
                            if (colCurr == curr) {
                                if (colPrev) {
                                    colPrev->nextdown = colCurr->nextdown;
                                }
                                else {
                                    hCol[curr->col] = colCurr->nextdown;
                                }
                                break;
                            }
                            colPrev = colCurr;
                            colCurr = colCurr->nextdown;
                        }
                    }
                    delete curr;
                    curr = nextNode; // Переход к следующему элементу
                } while (curr != hRow[i]); // Прерывание цикла после удаления всех элементов строки
                hRow[i] = nullptr; // Строка очищена
            }
        }
    }

    delete[] hRow;
    delete[] hCol;
}




void SparseMatrix::add(int val, int row, int col) {
    if (row >= size || col >= size || row < 0 || col < 0) {
        cout << "Ошибка: неверные координаты элемента!" << endl;
        return;
    }

    NODE* newNode = new NODE(val, row, col);

    // === Добавление в строку ===
    if (!hRow[row]) {
        // Строка пуста, добавляем первый элемент
        hRow[row] = newNode;
        newNode->nextright = newNode; // Закольцовываем список
    }
    else if (hRow[row]->nextright == hRow[row]) {
        // Строка содержит один элемент
        if (col < hRow[row]->col) {
            // Новый элемент становится первым
            newNode->nextright = hRow[row];
            hRow[row]->nextright = newNode;
            hRow[row] = newNode;
        }
        else if (col > hRow[row]->col) {
            // Новый элемент становится последним
            hRow[row]->nextright = newNode;
            newNode->nextright = hRow[row];
        }
        else {
            // Элемент уже существует, обновляем значение
            hRow[row]->data = val;
            delete newNode;
            return;
        }
    }
    else {
        // Строка содержит несколько элементов
        NODE* curr = hRow[row];
        NODE* prev = nullptr;

        do {
            prev = curr;
            curr = curr->nextright;

            // Если дошли до начала строки снова (закольцованный список), значит цикл завершён
            if (curr == hRow[row]) break;
        } while (curr != nullptr && curr->col < col);

        if (curr != hRow[row] && curr->col == col) {
            // Элемент уже существует, обновляем значение
            curr->data = val;
            delete newNode;
            return;
        }

        prev->nextright = newNode;
        newNode->nextright = curr;

        if (curr == hRow[row] && col < hRow[row]->col) {
            // Новый элемент становится первым
            hRow[row] = newNode;
        }
    }

    // === Добавление в столбец ===
    if (!hCol[col]) {
        // Столбец пуст, добавляем первый элемент
        hCol[col] = newNode;
        newNode->nextdown = newNode; // Закольцовываем список
    }
    else if (hCol[col]->nextdown == hCol[col]) {
        // Столбец содержит один элемент
        if (row < hCol[col]->row) {
            // Новый элемент становится первым
            newNode->nextdown = hCol[col];
            hCol[col]->nextdown = newNode;
            hCol[col] = newNode;
        }
        else if (row > hCol[col]->row) {
            // Новый элемент становится последним
            hCol[col]->nextdown = newNode;
            newNode->nextdown = hCol[col];
        }
        else {
            // Элемент уже существует, обновляем значение
            hCol[col]->data = val;
            delete newNode;
            return;
        }
    }
    else {
        // Столбец содержит несколько элементов
        NODE* curr = hCol[col];
        NODE* prev = nullptr;

        do {
            prev = curr;
            curr = curr->nextdown;

            // Если дошли до начала столбца снова (закольцованный список), значит цикл завершён
            if (curr == hCol[col]) break;
        } while (curr != nullptr && curr->row < row);

        if (curr != hCol[col] && curr->row == row) {
            // Элемент уже существует, обновляем значение
            curr->data = val;
            delete newNode;
            return;
        }

        prev->nextdown = newNode;
        newNode->nextdown = curr;

        if (curr == hCol[col] && row < hCol[col]->row) {
            // Новый элемент становится первым
            hCol[col] = newNode;
        }
    }
}




int SparseMatrix::get(int row, int col) const {
    if (row >= size || col >= size || row < 0 || col < 0) {
        cout << "Ошибка в координатах" << endl;
        return 0;
    }

    NODE* current = hRow[row];
    if (!current) return 0; // Если строка пустая

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

                // Суммируем значение в текущей позиции (row, col)
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
                        current = current->nextright; // Переход к следующему элементу строки
                        break;
                    }
                    current = current->nextright; // Циклично проходим строку
                } while (current != matrix.hRow[i]);
            }

            if (!found) {
                os << setw(3) << 0 << " "; // Если элемент не найден, выводим 0
            }
        }
        os << endl;
    }
    return os;
}
