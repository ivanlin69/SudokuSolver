#include "sudokuSolver.hpp"
#include <iostream>
#include "time.h"

int main() {

    std::string board = "090700860031005020806000000007050006000307000500010700000000109020600350054008070";

    SudokuSolver sudokuSolver;
    sudokuSolver.readBoard(board);
    //sudokuSolver.printStatus();

    clock_t a,b;
    a=clock();
    //sudokuSolver.inferAC3();
    //sudokuSolver.inferAC3Improved();
    sudokuSolver.inferAC3Guessing();
    b=clock();
    sudokuSolver.printStatus();
    std::cout << double(b)/CLOCKS_PER_SEC << std::endl;

    return 0;
}
