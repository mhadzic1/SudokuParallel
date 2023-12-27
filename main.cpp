#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>
#include <stack>

using namespace std;

const int N = 9;

void printBoard(int **board) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if((j % 3 == 0 && j > 2) || j == 3)
                cout << "| ";
            cout << board[i][j] << " ";
        }
        cout << endl;
        if((i % 3 == 2 && i > 2 && i != N-1) || i == 2){
            for(int k = 0; k < 3*N-6; k++)
                cout << "-";
            cout << endl;
        }
    }
}

bool isSafe(int **board, int row, int col, int num) {
    // Check if the number is already present in the row or column
    for (int i = 0; i < N; i++) {
        if (board[row][i] == num || board[i][col] == num) {
            return false;
        }
    }

    // Check if the number is present in the 3x3 grid
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
    for (row = 0; row < N; row++) {
        for (col = 0; col < N; col++) {
            if (board[row][col] == 0) {
                return true;
            }
        }
    }
    return false;
}

bool solveSudokuHelper(int **board, int row, int col);

bool solveSudoku(int **board) {
    return solveSudokuHelper(board, 0, 0);
}

bool solveSudokuHelper(int **board, int row, int col) {
    if (row == N)
        return true;

    int nextRow = (col == N - 1) ? row + 1 : row;
    int nextCol = (col == N - 1) ? 0 : col + 1;

    if (board[row][col] != 0)
        return solveSudokuHelper(board, nextRow, nextCol);

    for (int num = 1; num <= N; num++) {
        if (isSafe(board, row, col, num)) {
            board[row][col] = num;

            if (solveSudokuHelper(board, nextRow, nextCol))
                return true;

            board[row][col] = 0;
        }
    }

    return false;
}


bool solveSudokuStandard(int **board) {
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            if (board[row][col] == 0) {
                for (int num = 1; num <= N; num++) {
                    if (isSafe(board, row, col, num)) {
                        board[row][col] = num;

                        if (solveSudoku(board)) {
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
            if (solveSudoku(boardCopy)) {
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

    // Test Sudoku table
    int testSudoku[N][N] = {
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
            sudoku[i][j] = testSudoku[i][j];
        }
    }

    cout << "Testna Sudoku tabla:\n";
    printBoard(sudoku);

    auto start = chrono::high_resolution_clock::now();

    if (solveSudokuStandard(sudoku)) {
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;

        cout << "\nRijesena Sudoku tabla:\n";
        printBoard(sudoku);

        cout << "\nVrijeme potrebno za rjesavanje uz standardnu funkciju: " << duration.count() * 1000 << " milisekundi\n" << endl << endl;

    } else {
        cout << "\nNije moguce rijesiti Sudoku problem.\n";
    }

    int **sudoku2 = createBoard();

    // Test Sudoku table
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

    cout << "Testna Sudoku tabla:\n";
    printBoard(sudoku2);

    auto start2 = chrono::high_resolution_clock::now();

    if (solveSudoku(sudoku2)) {
        auto end2 = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end2 - start2;

        cout << "\nRijesena Sudoku tabla:\n";
        printBoard(sudoku);

        cout << "\nVrijeme potrebno za rjesavanje uz optimizovanu funkciju: " << duration.count() * 1000 << " milisekundi\n";

    } else {
        cout << "\nNije moguce rijesiti Sudoku problem.\n";
    }

    deleteBoard(sudoku);

    return 0;
}




