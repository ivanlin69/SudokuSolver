#include "sudokuSolver.hpp"
#include <iostream>

int main() {

    std::string board = "000700003096000000200850000170204036060070040082603510000017008000000250900002000";

    SudokuSolver sudokuSolver;
    sudokuSolver.readBoard(board);

    //sudokuSolver.inferAC3();
    sudokuSolver.inferAC3Guessing();
    sudokuSolver.printStatus();

    return 0;
}
