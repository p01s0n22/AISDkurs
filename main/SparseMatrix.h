#include <cstdlib> 
#include <ctime>  
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>

using namespace std;

template <typename T, T *T:: *Next, unsigned T:: *Order>
class IntrusiveOrderedCircularLinkedList {
public:
    bool empty() const {
        return head == nullptr;
    }

    std::pair<bool, T *> insert(T *value) {
        assert(value->*Next == nullptr);

        if (empty()) {
            head = value;
            value->*Next = head;
            return { true, value };
        }

        for (T *prev = nullptr, *node = head;; prev = node, node = node->*Next) {
            if (node->*Order == value->*Order) {
                return { false, node };
            }

            if (node->*Order > value->*Order) {
                if (prev) {
                    prev->*Next = value;
                    value->*Next = node;
                    return { true, value };
                }

                value->*Next = head;
                while (node->*Next != head) {
                    node = node->*Next;
                }
                node->*Next = value;
                head = value;
                return { true, value };
            }

            if (node->*Next == head) {
                node->*Next = value;
                value->*Next = head;
                return { true, value };
            }
        }
    }

    T *find(unsigned order) const {
        if (empty()) {
            return nullptr;
        }

        for (T *node = head;; node = node->*Next) {
            if (node->*Order == order) {
                return node;
            }

            if (node->*Order > order || node->*Next == head) {
                return nullptr;
            }
        }
    }

    T *findEqualOrGreater(unsigned order) const {
        if (empty()) {
            return nullptr;
        }

        for (T *node = head;; node = node->*Next) {
            if (node->*Order >= order) {
                return node;
            }

            if (node->*Next == head) {
                return nullptr;
            }
        }
    }

    T *erase(unsigned order) {
        if (empty()) {
            return nullptr;
        }

        for (T *prev = nullptr, *node = head;; prev = node, node = node->*Next) {
            if (node->*Order == order) {
                if (prev) {
                    prev->*Next = node->*Next;
                    return node;
                }

                if (head->*Next == head) {
                    head = nullptr;
                    return node;
                }

                while (node->*Next != head) {
                    node = node->*Next;
                }
                node->*Next = head->*Next;
                node = head;
                head = head->*Next;
                return node;
            }

            if (node->*Order > order || node->*Next == head) {
                return nullptr;
            }
        }
    }

    void reset(bool deleteNodes) {
        if (empty()) {
            return;
        }

        if (deleteNodes) {
            T *node = head;
            do {
                T *temp = node;
                node = node->*Next;
                delete temp;
            } while (node != head);
        }

        head = nullptr;
    }

    class Iterator {
    public:
        Iterator &operator++() {
            assert(current);
            current = current->*Next;
            if (current == head) {
                current = nullptr;
            }
            return *this;
        }

        T *operator*() const {
            assert(current);
            return current;
        }

        bool operator==(const Iterator &that) const {
            assert(head == that.head);
            return current == that.current;
        }

        bool operator!=(const Iterator &that) const {
            return !(*this == that);
        }

    private:
        friend class IntrusiveOrderedCircularLinkedList;

        Iterator(T *head, T *current)
            : head(head)
            , current(current) {
        }

        T *head;
        T *current;
    };

    Iterator begin() const {
        return { head, head };
    }

    Iterator end() const {
        return { head, nullptr };
    }

private:
    T *head = nullptr;
};

namespace detail {
    template <typename T, T *T:: *TNext, unsigned T:: *TOrder, typename U, U *U:: *UNext, unsigned U:: *UOrder>
    class Zip {
    public:
        using TList = IntrusiveOrderedCircularLinkedList<T, TNext, TOrder>;
        using UList = IntrusiveOrderedCircularLinkedList<U, UNext, UOrder>;

        using TIterator = typename TList::Iterator;
        using UIterator = typename UList::Iterator;

        Zip(TList tList, UList uList)
            : tBegin(tList.begin())
            , tEnd(tList.end())
            , uBegin(uList.begin())
            , uEnd(uList.end()) {
        }

        class Iterator {
        public:
            Iterator &operator++() {
                if (uBegin == uEnd) {
                    assert(tBegin != tEnd);
                    ++tBegin;
                }
                else if (tBegin == tEnd) {
                    assert(uBegin != uEnd);
                    ++uBegin;
                }
                else if ((*tBegin)->*TOrder < (*uBegin)->*UOrder) {
                    ++tBegin;
                }
                else if ((*tBegin)->*TOrder > (*uBegin)->*UOrder) {
                    ++uBegin;
                }
                else {
                    ++tBegin;
                    ++uBegin;
                }
                return *this;
            }

            std::pair<T *, U *> operator*() const {
                if (uBegin == uEnd) {
                    assert(tBegin != tEnd);
                    return { *tBegin, nullptr };
                }
                if (tBegin == tEnd) {
                    assert(uBegin != uEnd);
                    return { nullptr, *uBegin };
                }
                if ((*tBegin)->*TOrder < (*uBegin)->*UOrder) {
                    return { *tBegin, nullptr };
                }
                if ((*tBegin)->*TOrder > (*uBegin)->*UOrder) {
                    return { nullptr, *uBegin };
                }
                return { *tBegin, *uBegin };
            }

            bool operator!=(const Iterator &that) const {
                assert(tEnd == that.tEnd);
                assert(uEnd == that.uEnd);
                return tBegin != that.tBegin || uBegin != that.uBegin;
            }

        private:
            friend class Zip;
            Iterator(TIterator tBegin, TIterator tEnd, UIterator uBegin, UIterator uEnd)
                : tBegin(tBegin)
                , tEnd(tEnd)
                , uBegin(uBegin)
                , uEnd(uEnd) {
            }

            TIterator tBegin, tEnd;
            UIterator uBegin, uEnd;
        };

        Iterator begin() {
            return { tBegin, tEnd, uBegin, uEnd };
        }

        Iterator end() {
            return { tEnd, tEnd, uEnd, uEnd };
        }

    private:
        TIterator tBegin, tEnd;
        UIterator uBegin, uEnd;
    };
} // namespace detail

template <typename T, T *T:: *TNext, unsigned T:: *TOrder, typename U, U *U:: *UNext, unsigned U:: *UOrder>
detail::Zip<T, TNext, TOrder, U, UNext, UOrder> zip(IntrusiveOrderedCircularLinkedList<T, TNext, TOrder> tList,
    IntrusiveOrderedCircularLinkedList<U, UNext, UOrder> uList) {
    return { tList, uList };
}

class SparseMatrix {
    struct NODE {
        int data;
        unsigned row, col;
        NODE *nextdown;
        NODE *nextright;

        NODE(int data, unsigned r, unsigned c)
            : data(data)
            , row(r)
            , col(c)
            , nextright(nullptr)
            , nextdown(nullptr) {
        }
    };

    unsigned size;

    using RowsList = IntrusiveOrderedCircularLinkedList<NODE, &NODE::nextright, &NODE::col>;
    RowsList* rows;

    using ColsList = IntrusiveOrderedCircularLinkedList<NODE, &NODE::nextdown, &NODE::row>;
    ColsList* cols;

public:
    SparseMatrix();
    SparseMatrix(unsigned size);
    ~SparseMatrix();
    SparseMatrix(SparseMatrix&& other);
    SparseMatrix& operator=(SparseMatrix&& other);
private:
    void clear();
public:
    void set(int data, unsigned row, unsigned col);
    int get(unsigned row, unsigned col) const;
    void inputMatrix();

    SparseMatrix operator+(const SparseMatrix& other) const;

    SparseMatrix operator-(const SparseMatrix& other) const;

    SparseMatrix operator*(const SparseMatrix& other) const;

    void swapRows(unsigned row1, unsigned row2);

    SparseMatrix submatrix(unsigned delRow, unsigned delCol) const;

    double determinant();

    void additionWithFactor(int factor, unsigned targetRow, unsigned pivotRow);

    SparseMatrix::NODE* getElement(int row, int col) const;

    void multiplicationByNumber(int number, unsigned row);

    void generateRandomMatrix(unsigned size, int density);

    void RandomSparseMatrix(unsigned nonZeroElements);
    friend ostream& operator<<(ostream& os, const SparseMatrix& matrix);
};