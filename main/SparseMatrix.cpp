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
    if (val == 0) return; // Ничего не делаем, если значение равно 0

    if (row >= size || col >= size || row < 0 || col < 0) {
        throw std::out_of_range("Неверные координаты!");
    }

    NODE* newNode = new NODE(val, row, col);

    // Добавление в строку
    if (!hRow[row]) { // Если строка пустая
        hRow[row] = newNode;
        newNode->nextright = newNode; // Циклическая связь
    }
    else {
        NODE* current = hRow[row];
        NODE* prev = nullptr;

        do {
            if (current->col == col) { // Если элемент с таким столбцом уже есть
                current->data = val;  // Обновляем значение
                delete newNode;       // Новый узел больше не нужен
                return;
            }
            if (current->col > col) break; // Найдено место для вставки
            prev = current;
            current = current->nextright;
        } while (current != hRow[row]);

        if (prev == nullptr) { // Вставка в начало строки
            newNode->nextright = current;

            // Найти последний элемент для обновления его указателя
            NODE* last = hRow[row];
            while (last->nextright != hRow[row]) {
                last = last->nextright;
            }
            last->nextright = newNode;
            hRow[row] = newNode; // Обновляем голову
        }
        else { // Вставка между узлами
            prev->nextright = newNode;
            newNode->nextright = current;
        }
    }

    // Добавление в столбец
    if (!hCol[col]) { // Если столбец пустой
        hCol[col] = newNode;
        newNode->nextdown = newNode; // Циклическая связь
    }
    else {
        NODE* current = hCol[col];
        NODE* prev = nullptr;

        do {
            if (current->row > row) break; // Найдено место для вставки
            prev = current;
            current = current->nextdown;
        } while (current != hCol[col]);

        if (prev == nullptr) { // Вставка в начало столбца
            newNode->nextdown = current;

            // Найти последний элемент для обновления его указателя
            NODE* last = hCol[col];
            while (last->nextdown != hCol[col]) {
                last = last->nextdown;
            }
            last->nextdown = newNode;
            hCol[col] = newNode; // Обновляем голову
        }
        else { // Вставка между узлами
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
