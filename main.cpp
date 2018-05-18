#include <iostream>
#include <array>
#include <bitset>
#include <cassert>
#include <vector>

#define assertIndex(x) assert(0 <= x < 9)
#define assertValue(x) assert(1 <= x < 10)

using namespace std;

enum IndexKind {
    ROW,
    COLUMN,
    BLOCK
};

struct Sudoku;

struct Move {
    Move(int row, int column, int digit)
        : m_row(row)
        , m_column(column)
        , m_digitIndex(digit - 1)
        , m_usedDigitsIndexRow(indexUsedDigits(m_digitIndex, ROW, row))
        , m_usedDigitsIndexColumn(indexUsedDigits(m_digitIndex, COLUMN, column))
        , m_usedDigitsIndexBlock(indexUsedDigits(m_digitIndex, BLOCK, indexBlock(row, column))) {
        assertIndex(row);
        assertIndex(column);
        assertValue(digit);
    }

    int m_row, m_column, m_digitIndex, m_usedDigitsIndexRow, m_usedDigitsIndexColumn, m_usedDigitsIndexBlock;

    inline constexpr int indexBlock(int row, int column) const{
        return (column / 3) + 3 * (row / 3);
    }

    inline constexpr int indexUsedDigits(int digitIndex, IndexKind indexKind, int index) {
        return digitIndex + 9 * (indexKind + 3 * index);
    }

    friend class Sudoku;
};

ostream& operator<<(ostream & os, const Move & move) {
    os << "[" << move.m_row << "," << move.m_column << "]=" << (move.m_digitIndex + 1);
    return os;
}

// TODO optimize iteration over Moves by incremental index computation
// TODO in naive iteration, some checks might be done several times?

struct Sudoku {

    void perform(const Move & move) {
        m_values[indexGrid(move.m_row, move.m_column)] = move.m_digitIndex + 1;
        m_usedDigits[move.m_usedDigitsIndexRow] = true;
        m_usedDigits[move.m_usedDigitsIndexColumn] = true;
        m_usedDigits[move.m_usedDigitsIndexBlock] = true;
    }

    void unperform(const Move & move) {
        m_values[indexGrid(move.m_row, move.m_column)] = 0;
        m_usedDigits[move.m_usedDigitsIndexRow] = false;
        m_usedDigits[move.m_usedDigitsIndexColumn] = false;
        m_usedDigits[move.m_usedDigitsIndexBlock] = false;
    }

    int operator()(int row, int column) const {
        return m_values[indexGrid(row, column)];
    }

    bool isEmpty(int row, int column) const {
        return m_values[indexGrid(row, column)] == 0;
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

    inline constexpr int indexGrid(int row, int column) const {
        return column + 9 * row;
    }

};

// TODO optimize by also incrementing block
inline bool increment(int & row, int & column) {
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
bool solve(Sudoku & sudoku, int row, int column) {
    while (!sudoku.isEmpty(row, column) && increment(row, column)) {}
    if (row == 9) { // if increment failed
        return true;
    }
    assert(sudoku.isEmpty(row, column));
    for (int digit = 1; digit <= 9; ++digit) {
        Move move(row, column, digit);
        if (sudoku.isValid(move)) {
            sudoku.perform(move);
            // printSudoku(sudoku);
            int row2 = row;
            int column2 = column;
            increment(row2, column2); // result is implicitely checked in the next line
            if (row2 == 9) { // TODO this is ugly to have the termination check a second time
                return true;
            }
            if (solve(sudoku, row2, column2)) {
                return true; // propagate first solution
            }
            sudoku.unperform(move);
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
            Move move(row, column, digit);
            if (!sudoku.isValid(move)) {
                cerr << "invalid move: " << move << endl;
                printSudoku(sudoku);
                exit(1);
            }
            sudoku.perform(move);
        }
    }
    return sudoku;
}

int main() {
    Sudoku sudoku = readSudoku();
    cout << "input:" << endl;
    printSudoku(sudoku);
    if (solve(sudoku, 0, 0)) {
        cout << "output:" << endl;
        printSudoku(sudoku);
    } else {
        cout << "No solution found" << endl;
    }
    return 0;
}
