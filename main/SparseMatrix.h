#include <cstdlib> 
#include <ctime>  
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

class SparseMatrix {
    struct NODE {
    public:
        int data;
        int row, col;
        NODE* nextdown;
        NODE* nextright;

        NODE(double v, int r, int c) : data(v), row(r), col(c), nextdown(nullptr), nextright(nullptr) {}
    };
    NODE** hRow;
    NODE** hCol;
    int size;

public:
    SparseMatrix();
    SparseMatrix(size_t n);
    ~SparseMatrix();
    SparseMatrix(const SparseMatrix& other);
    SparseMatrix& operator=(const SparseMatrix& other);
    void clear();
    void copyFrom(const SparseMatrix& other);
    void add(double val, int row, int col);
    int get(int row, int col) const;
    void inputMatrix();

    SparseMatrix operator+(const SparseMatrix& other) const;

    SparseMatrix operator-(const SparseMatrix& other) const;

    SparseMatrix operator*(const SparseMatrix& other) const;

    void swapRows(int row1, int row2);

    SparseMatrix submatrix(int delRow, int delCol) const;

    double determinant();

    void additionWithFactor(int numerator, int targetRow, int pivotRow);

    /*void additionWithString(int value, int rowToAdd, int rowToSubtract);*/

    SparseMatrix::NODE* getElement(int row, int col) const;

    /*void multiplicationByNumber(int value, int row);*/

    void generateRandomMatrix(size_t n, int density);

    //SparseMatrix::NODE* findElement(int targetRow, int col);

    void addElement(int targetRow, int col, int value);

    friend ostream& operator<<(ostream& os, const SparseMatrix& matrix);
};