#include <iostream>
#include <array>
#include <bitset>
#include <cassert>
#include <vector>

#define assertIndex(x) assert(0 <= x < 9)
#define assertValue(x) assert(1 <= x < 10)

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

using namespace std;

enum IndexKind {
    ROW,
    COLUMN,
    BLOCK
};

struct Sudoku;

inline constexpr int divideBy3_0(int x) {
    return x / 3;
}

// slower than integer division
inline constexpr int divideBy3_1(int x) {
    int result = 0;
    if (x >= 3) {
        result++;
    }
    if (x >= 6) {
        result++;
    }
    return result;
}

inline constexpr int divideBy3_2(int x) {
    // see http://homepage.divms.uiowa.edu/~jones/bcd/divide.html
    return (x * 0xAAAB) >> 17;
}

inline constexpr int divideBy3(int x) {
    // see http://homepage.divms.uiowa.edu/~jones/bcd/divide.html

//    Q = ((A >>  2) + A) >> 1; /* Q = A*0.101 */
//    Q = ((Q >>  4) + Q)     ; /* Q = A*0.1010101 */
//    Q = ((Q >>  8) + Q) >> 1; /* Q = A*0.0101010101010101 */
    x = x + 1;
    int q = ((x >> 2) + x) >> 1;
    q = ((q >> 4) + q);
    q = ((q >> 8) + q) >> 1;
    return q;
}

inline constexpr int indexBlock(int row, int column) {
    return divideBy3(column) + 3 * divideBy3(row);
}

struct Move {
    Move(int row, int column, int digit, int indexBlock)
        : m_digitIndex(digit - 1)
        , m_usedDigitsIndexRow(indexUsedDigits(m_digitIndex, ROW, row))
        , m_usedDigitsIndexColumn(indexUsedDigits(m_digitIndex, COLUMN, column))
        , m_usedDigitsIndexBlock(indexUsedDigits(m_digitIndex, BLOCK, indexBlock)) {
        assertIndex(row);
        assertIndex(column);
        assertValue(digit);
    }

    int m_digitIndex, m_usedDigitsIndexRow, m_usedDigitsIndexColumn, m_usedDigitsIndexBlock;

    inline constexpr int indexUsedDigits(int digitIndex, IndexKind indexKind, int index) {
        return digitIndex + 9 * (indexKind + 3 * index);
    }

    friend class Sudoku;
};

// TODO optimize iteration over Moves by incremental index computation
// TODO in naive iteration, some checks might be done several times?

inline constexpr int indexGrid(int row, int column) {
    return column + 9 * row;
}

struct Sudoku {

    void perform(int index, const Move & move) {
        m_values[index] = move.m_digitIndex + 1;
        m_usedDigits[move.m_usedDigitsIndexRow] = true;
        m_usedDigits[move.m_usedDigitsIndexColumn] = true;
        m_usedDigits[move.m_usedDigitsIndexBlock] = true;
    }

    void unperform(int index, const Move & move) {
        m_values[index] = 0;
        m_usedDigits[move.m_usedDigitsIndexRow] = false;
        m_usedDigits[move.m_usedDigitsIndexColumn] = false;
        m_usedDigits[move.m_usedDigitsIndexBlock] = false;
    }

    int operator()(int row, int column) const {
        return m_values[indexGrid(row, column)];
    }

    bool isEmpty(int index) const {
        return m_values[index] == 0;
    }

    bool isValid(const Move & move) const {
        return !m_usedDigits[move.m_usedDigitsIndexRow]
                && !m_usedDigits[move.m_usedDigitsIndexColumn]
                && !m_usedDigits[move.m_usedDigitsIndexBlock];
    }

private:

    // Values of the grid (1-9).
    // Indexed via indexGrid()
    array<uint8_t, 9*9> m_values {0}; // TODO why is this default value even necessary?

    // Digits which have been used in the grid.
    // Used to answer the question whether a given digit is used in a row/column/block.
    // Indexed via indexUsedDigits() + digitIndex
    bitset<3 * 9 * 9> m_usedDigits;

};

// TODO optimize by also incrementing block
inline bool increment(int & row, int & column, int & index) {
    // TODO just increment index, then divide by 9 (if needed)
    // TODO "next empty bit" operation?
    index++;
    column++;
    if (column >= 9) {
        column = 0;
        row++;
        if (row >= 9) {
            return false;
        }
    }
    return true;
}

void printSudoku(const Sudoku & sudoku) {
    for (int row = 0; row < 9; ++row) {
        for (int column = 0; column < 9; ++column) {
            cout << sudoku(row, column) << " ";
        }
        cout << endl;
    }
}

// returns true if a solution has been found
bool solve(Sudoku & sudoku, int row, int column, int index) {
    while (!sudoku.isEmpty(index) && increment(row, column, index)) {}
    if (row == 9) { // if increment failed
        return true;
    }
    assert(sudoku.isEmpty(row, column));

    int block = indexBlock(row, column);
    for (int digit = 1; digit <= 9; ++digit) {
        Move move(row, column, digit, block); // TODO incremental computation of Move
        if (sudoku.isValid(move)) {
            sudoku.perform(index, move);
            // printSudoku(sudoku);
            if (index == 81) { // TODO this is ugly to have the termination check a second time
                return true;
            }

            int row2 = row;
            int column2 = column;
            int index2 = index;
            // TODO remove this increment, it should already be called in child, if order is correct?
            increment(row2, column2, index2); // result is implicitely checked in the next line
            if (solve(sudoku, row2, column2,  index2)) {
                return true; // propagate first solution
            }
            sudoku.unperform(index, move);
        }
    }
    return false;
}

Sudoku readSudoku() {
    Sudoku sudoku;
    for (int row = 0; row < 9; ++row) {
        for (int column = 0; column < 9; ++column) {
            int digit;
            cin >> digit;
            if (digit == 0) {
                continue;
            }
            int block = indexBlock(row, column);
            Move move(row, column, digit, block);
            if (!sudoku.isValid(move)) {
                cerr << "invalid move: [" << row << ", " << column << "]=" << digit<< endl;
                printSudoku(sudoku);
                exit(1);
            }
            sudoku.perform(indexGrid(row, column), move);
        }
    }
    return sudoku;
}

int main() {
//    for (int i = 0; i < 9; ++ i) {
//        cout << divideBy3(i) << endl;
//    }
//    return 0;
    Sudoku sudoku = readSudoku();
    cout << "input:" << endl;
    printSudoku(sudoku);
    if (solve(sudoku, 0, 0, 0)) {
        cout << "output:" << endl;
        printSudoku(sudoku);
    } else {
        cout << "No solution found" << endl;
    }
    return 0;
}
