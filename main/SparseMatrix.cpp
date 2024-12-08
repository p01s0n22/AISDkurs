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

            // ���� � ������ ������ ���� �������
            if (curr->nextright == curr) {
                // ������� ������������ ������� � ������
                hRow[i] = nullptr; // ������ ������
                delete curr;
            }
            else {
                // ���� � ������ ��������� ���������
                do {
                    nextNode = curr->nextright; // ������� � ���������� ��������
                    // ������� ����� �� �������
                    if (curr->nextdown) {
                        NODE* colCurr = hCol[curr->col];
                        NODE* colPrev = nullptr;

                        // ������� ����� � �������
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
                    curr = nextNode; // ������� � ���������� ��������
                } while (curr != hRow[i]); // ���������� ����� ����� �������� ���� ��������� ������
                hRow[i] = nullptr; // ������ �������
            }
        }
    }

    delete[] hRow;
    delete[] hCol;
}




void SparseMatrix::add(int val, int row, int col) {
    if (row >= size || col >= size || row < 0 || col < 0) {
        cout << "������: �������� ���������� ��������!" << endl;
        return;
    }

    NODE* newNode = new NODE(val, row, col);

    // === ���������� � ������ ===
    if (!hRow[row]) {
        // ������ �����, ��������� ������ �������
        hRow[row] = newNode;
        newNode->nextright = newNode; // �������������� ������
    }
    else if (hRow[row]->nextright == hRow[row]) {
        // ������ �������� ���� �������
        if (col < hRow[row]->col) {
            // ����� ������� ���������� ������
            newNode->nextright = hRow[row];
            hRow[row]->nextright = newNode;
            hRow[row] = newNode;
        }
        else if (col > hRow[row]->col) {
            // ����� ������� ���������� ���������
            hRow[row]->nextright = newNode;
            newNode->nextright = hRow[row];
        }
        else {
            // ������� ��� ����������, ��������� ��������
            hRow[row]->data = val;
            delete newNode;
            return;
        }
    }
    else {
        // ������ �������� ��������� ���������
        NODE* curr = hRow[row];
        NODE* prev = nullptr;

        do {
            prev = curr;
            curr = curr->nextright;

            // ���� ����� �� ������ ������ ����� (�������������� ������), ������ ���� ��������
            if (curr == hRow[row]) break;
        } while (curr != nullptr && curr->col < col);

        if (curr != hRow[row] && curr->col == col) {
            // ������� ��� ����������, ��������� ��������
            curr->data = val;
            delete newNode;
            return;
        }

        prev->nextright = newNode;
        newNode->nextright = curr;

        if (curr == hRow[row] && col < hRow[row]->col) {
            // ����� ������� ���������� ������
            hRow[row] = newNode;
        }
    }

    // === ���������� � ������� ===
    if (!hCol[col]) {
        // ������� ����, ��������� ������ �������
        hCol[col] = newNode;
        newNode->nextdown = newNode; // �������������� ������
    }
    else if (hCol[col]->nextdown == hCol[col]) {
        // ������� �������� ���� �������
        if (row < hCol[col]->row) {
            // ����� ������� ���������� ������
            newNode->nextdown = hCol[col];
            hCol[col]->nextdown = newNode;
            hCol[col] = newNode;
        }
        else if (row > hCol[col]->row) {
            // ����� ������� ���������� ���������
            hCol[col]->nextdown = newNode;
            newNode->nextdown = hCol[col];
        }
        else {
            // ������� ��� ����������, ��������� ��������
            hCol[col]->data = val;
            delete newNode;
            return;
        }
    }
    else {
        // ������� �������� ��������� ���������
        NODE* curr = hCol[col];
        NODE* prev = nullptr;

        do {
            prev = curr;
            curr = curr->nextdown;

            // ���� ����� �� ������ ������� ����� (�������������� ������), ������ ���� ��������
            if (curr == hCol[col]) break;
        } while (curr != nullptr && curr->row < row);

        if (curr != hCol[col] && curr->row == row) {
            // ������� ��� ����������, ��������� ��������
            curr->data = val;
            delete newNode;
            return;
        }

        prev->nextdown = newNode;
        newNode->nextdown = curr;

        if (curr == hCol[col] && row < hCol[col]->row) {
            // ����� ������� ���������� ������
            hCol[col] = newNode;
        }
    }
}




int SparseMatrix::get(int row, int col) const {
    if (row >= size || col >= size || row < 0 || col < 0) {
        cout << "������ � �����������" << endl;
        return 0;
    }

    NODE* current = hRow[row];
    if (!current) return 0; // ���� ������ ������

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
    cout << "������� ������ �������: ";
    cin >> n;
    if (n <= 0) {
        cout << "������: ������ ������� ������ ���� ������������� ������!" << endl;
        return;
    }
    *this = SparseMatrix(n);

    cout << "������� ������� ��������� (������� ����):" << endl;
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
        cerr << "������ �������� �����!" << endl;
        return;
    }

    int n;
    file >> n;
    if (n <= 0) {
        cout << "������ ������� ������ ���� �������������!" << endl;
        return;
    }
    *this = SparseMatrix(n);
    cout << "���������� ������� �� �����..." << endl;
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
    cout << "������� ������� �������." << endl;
}

SparseMatrix SparseMatrix::operator+(const SparseMatrix& other) const {
    if (this->size != other.size) {
        throw std::invalid_argument("������� ������ ��������!");
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
        throw std::invalid_argument("������� ������ ��������!");
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
        throw std::invalid_argument("������� ������ ��������!");
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

                // ��������� �������� � ������� ������� (row, col)
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
        throw std::out_of_range("�������� ����� ������!");
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
                        current = current->nextright; // ������� � ���������� �������� ������
                        break;
                    }
                    current = current->nextright; // �������� �������� ������
                } while (current != matrix.hRow[i]);
            }

            if (!found) {
                os << setw(3) << 0 << " "; // ���� ������� �� ������, ������� 0
            }
        }
        os << endl;
    }
    return os;
}
