#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>
#include <stack>
#include <unordered_set>

using namespace std;

const int N = 9;

void printBoard(int **board) {
    // Function to print the Sudoku board
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if ((j % 3 == 0 && j > 2) || j == 3)
                cout << "| ";
            cout << board[i][j] << " ";
        }
        cout << endl;
        if ((i % 3 == 2 && i > 2 && i != N - 1) || i == 2) {
            for (int k = 0; k < 3 * N - 6; k++)
                cout << "-";
            cout << endl;
        }
    }
}

bool isSafe(int **board, int row, int col, int num) {
    // Function to check if placing 'num' in the given position is safe
    for (int i = 0; i < N; i++) {
        if (board[row][i] == num || board[i][col] == num) {
            return false;
        }
    }

    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i + startRow][j + startCol] == num) {
                return false;
            }
        }
    }

    return true;
}

bool findEmptyLocation(int **board, int &row, int &col) {
    // Function to find an empty location in the Sudoku board
    for (row = 0; row < N; row++) {
        for (col = 0; col < N; col++) {
            if (board[row][col] == 0) {
                return true;
            }
        }
    }
    return false;
}

bool solveSudokuStandard(int **board) {
    // Standard backtracking approach to solve Sudoku
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            if (board[row][col] == 0) {
                for (int num = 1; num <= N; num++) {
                    if (isSafe(board, row, col, num)) {
                        board[row][col] = num;

                        if (solveSudokuStandard(board)) {
                            return true;
                        }

                        board[row][col] = 0;
                    }
                }
                return false; // No appropriate number for this position
            }
        }
    }
    return true; // Sudoku solved
}

bool solveSudokuAdvanced(int **board, int row, int col, bool usedInRow[N][N], bool usedInCol[N][N]) {
    // Advanced approach to solve Sudoku with memory of row and column
    if (row == N)
        return true;

    int nextRow = (col == N - 1) ? row + 1 : row;
    int nextCol = (col == N - 1) ? 0 : col + 1;

    if (board[row][col] != 0)
        return solveSudokuAdvanced(board, nextRow, nextCol, usedInRow, usedInCol);

    for (int num = 1; num <= N; num++) {
        if (!usedInRow[row][num - 1] && !usedInCol[col][num - 1] && isSafe(board, row, col, num)) {
            board[row][col] = num;
            usedInRow[row][num - 1] = true;
            usedInCol[col][num - 1] = true;

            if (solveSudokuAdvanced(board, nextRow, nextCol, usedInRow, usedInCol))
                return true;

            board[row][col] = 0;
            usedInRow[row][num - 1] = false;
            usedInCol[col][num - 1] = false;
        }
    }

    return false;
}

bool solveSudokuProHelper(int **board, int row, int col, bool usedInRow[N][N], bool usedInCol[N][N], bool usedInBox[N / 3][N / 3][N]);

bool solveSudokuPro(int **board) {
    bool usedInRow[N][N] = {false};
    bool usedInCol[N][N] = {false};
    bool usedInBox[N / 3][N / 3][N] = {false};
    return solveSudokuProHelper(board, 0, 0, usedInRow, usedInCol, usedInBox);
}

bool solveSudokuProHelper(int **board, int row, int col, bool usedInRow[N][N], bool usedInCol[N][N], bool usedInBox[N / 3][N / 3][N]) {
    if (row == N)
        return true;

    int nextRow = (col == N - 1) ? row + 1 : row;
    int nextCol = (col == N - 1) ? 0 : col + 1;

    if (board[row][col] != 0)
        return solveSudokuProHelper(board, nextRow, nextCol, usedInRow, usedInCol, usedInBox);

    for (int num = 1; num <= N; num++) {
        if (!usedInRow[row][num - 1] && !usedInCol[col][num - 1] && !usedInBox[row / 3][col / 3][num - 1]) {
            board[row][col] = num;
            usedInRow[row][num - 1] = true;
            usedInCol[col][num - 1] = true;
            usedInBox[row / 3][col / 3][num - 1] = true;

            if (solveSudokuProHelper(board, nextRow, nextCol, usedInRow, usedInCol, usedInBox))
                return true;

            board[row][col] = 0;
            usedInRow[row][num - 1] = false;
            usedInCol[col][num - 1] = false;
            usedInBox[row / 3][col / 3][num - 1] = false;
        }
    }

    return false;
}


void initializeBoard(int **board) {
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            board[i][j] = 0;
}

int** createBoard() {
    int** board = new int*[N];
    for (int i = 0; i < N; i++) {
        board[i] = new int[N];
    }
    return board;
}

void deleteBoard(int** board) {
    for (int i = 0; i < N; i++) {
        delete[] board[i];
    }
    delete[] board;
}

#include <unordered_set>

bool generateRandomSudoku(int **board, double probability = 500) {
    // Initialize random number generator with current time as seed
    std::mt19937 generator(42);

    // Create a pool of numbers for each empty cell
    std::vector<std::unordered_set<int>> numberPool(N * N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] == 0) {
                numberPool[i * N + j] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            }
        }
    }

    // Shuffle the order of empty cells
    std::vector<int> emptyCellIndices(N * N);
    std::iota(emptyCellIndices.begin(), emptyCellIndices.end(), 0);
    std::shuffle(emptyCellIndices.begin(), emptyCellIndices.end(), generator);

    // Keep attempting until a solvable puzzle is generated
    while (true) {
        // Make a copy of the original board
        int **boardCopy = createBoard();
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                boardCopy[i][j] = board[i][j];
            }
        }

        // Reset the record of used numbers
        // Initialize boolean arrays
        bool usedInRowCopy[N][N] = {false};
        bool usedInColCopy[N][N] = {false};
        bool usedInBoxCopy[N / 3][N / 3][N] = {false};

        // Iterate through each empty cell and add a number with the specified probability
        bool success = true;
        for (const auto &index : emptyCellIndices) {
            int i = index / N;
            int j = index % N;
            if (generator() % 100 < probability * 100 && !numberPool[index].empty()) {
                // Select a random number from the pool
                auto it = numberPool[index].begin();
                std::advance(it, generator() % numberPool[index].size());
                int num = *it;

                // Check if the selected number is not already used in the row, column, or 3x3 subgrid
                if (!usedInRowCopy[i][num - 1] && !usedInColCopy[j][num - 1] && !usedInBoxCopy[i / 3][j / 3][num - 1]) {
                    boardCopy[i][j] = num;
                    usedInRowCopy[i][num - 1] = true;
                    usedInColCopy[j][num - 1] = true;
                    usedInBoxCopy[i / 3][j / 3][num - 1] = true;

                    // Remove the selected number from the pool
                    numberPool[index].erase(num);
                } else {
                    // If the selected number cannot be placed, retry
                    success = false;
                    break;
                }
            }
        }

        printBoard(boardCopy);
        cout << endl;
        if (success) {
            // Check if the generated puzzle is solvable
            if (solveSudokuStandard(boardCopy)) {
                // Copy the solved board back to the original board
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) {
                        board[i][j] = boardCopy[i][j];
                    }
                }

                // Free the memory used by the copy
                deleteBoard(boardCopy);
                return true; // Puzzle generated successfully
            }
        }

        // Free the memory used by the copy
        deleteBoard(boardCopy);
    }

    return false; // This should never be reached
}



int main() {
    int **sudoku = createBoard();

    // Test Sudoku table 1
    int testSudoku1[N][N] = {
            {5, 3, 0, 0, 7, 0, 0, 0, 0},
            {6, 0, 0, 1, 9, 5, 0, 0, 0},
            {0, 9, 8, 0, 0, 0, 0, 6, 0},
            {8, 0, 0, 0, 6, 0, 0, 0, 3},
            {4, 0, 0, 8, 0, 3, 0, 0, 1},
            {7, 0, 0, 0, 2, 0, 0, 0, 6},
            {0, 6, 0, 0, 0, 0, 2, 8, 0},
            {0, 0, 0, 4, 1, 9, 0, 0, 5},
            {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    // Copying the table to a dynamically allocated sudoku table
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sudoku[i][j] = testSudoku1[i][j];
        }
    }

    cout << "Testna Sudoku tabla 1:\n";
    printBoard(sudoku);

    auto start1 = chrono::high_resolution_clock::now();

    if (solveSudokuStandard(sudoku)) {
        auto end1 = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end1 - start1;

        cout << "\nRijesena Sudoku tabla:\n";
        printBoard(sudoku);

        cout << "\nVrijeme potrebno za rjesavanje uz standardnu funkciju: " << duration.count() * 1000 << " milisekundi\n" << endl << endl;

    } else {
        cout << "\nNije moguce rijesiti Sudoku problem.\n";
    }

    int **sudoku2 = createBoard();

    // Test Sudoku table 2
    int testSudoku2[N][N] = {
            {5, 3, 0, 0, 7, 0, 0, 0, 0},
            {6, 0, 0, 1, 9, 5, 0, 0, 0},
            {0, 9, 8, 0, 0, 0, 0, 6, 0},
            {8, 0, 0, 0, 6, 0, 0, 0, 3},
            {4, 0, 0, 8, 0, 3, 0, 0, 1},
            {7, 0, 0, 0, 2, 0, 0, 0, 6},
            {0, 6, 0, 0, 0, 0, 2, 8, 0},
            {0, 0, 0, 4, 1, 9, 0, 0, 5},
            {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    // Copying the table to a dynamically allocated sudoku table
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sudoku2[i][j] = testSudoku2[i][j];
        }
    }

    cout << "Testna Sudoku tabla 2:\n";
    printBoard(sudoku2);

    auto start2 = chrono::high_resolution_clock::now();

    if (solveSudokuPro(sudoku2)) {
        auto end2 = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end2 - start2;

        cout << "\nRijesena Sudoku tabla:\n";
        printBoard(sudoku);

        cout << "\nVrijeme potrebno za rjesavanje uz PRO funkciju: " << duration.count() * 1000 << " milisekundi\n" <<endl << endl;

    } else {
        cout << "\nNije moguce rijesiti Sudoku problem.\n";
    }

    int **sudoku3 = createBoard();

    // Test Sudoku table 3
    int testSudoku3[N][N] = {
            {5, 3, 0, 0, 7, 0, 0, 0, 0},
            {6, 0, 0, 1, 9, 5, 0, 0, 0},
            {0, 9, 8, 0, 0, 0, 0, 6, 0},
            {8, 0, 0, 0, 6, 0, 0, 0, 3},
            {4, 0, 0, 8, 0, 3, 0, 0, 1},
            {7, 0, 0, 0, 2, 0, 0, 0, 6},
            {0, 6, 0, 0, 0, 0, 2, 8, 0},
            {0, 0, 0, 4, 1, 9, 0, 0, 5},
            {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    // Copying the table to a dynamically allocated sudoku table
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sudoku3[i][j] = testSudoku3[i][j];
        }
    }

    cout << "Testna Sudoku tabla 3:\n";
    printBoard(sudoku3);

    auto start3 = chrono::high_resolution_clock::now();

    bool usedInRow3[N][N] = {false};
    bool usedInCol3[N][N] = {false};

    if (solveSudokuAdvanced(sudoku3, 0, 0, usedInRow3, usedInCol3)) {
        auto end3 = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end3 - start3;

        cout << "\nRijesena Sudoku tabla:\n";
        printBoard(sudoku3);

        cout << "\nVrijeme potrebno za rjesavanje uz ADVANCED funkciju: " << duration.count() * 1000 << " milisekundi\n";

    } else {
        cout << "\nNije moguce rijesiti Sudoku problem.\n";
    }

    deleteBoard(sudoku);

    return 0;
}






