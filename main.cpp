#include "sudokuSolver.hpp"
#include <iostream>
#include "time.h"

int main() {

    //std::string board = "090700860031005020806000000007050006000307000500010700000000109020600350054008070";

    SudokuSolver sudokuSolver;
    sudokuSolver.welcomeMessage();
    //sudokuSolver.readBoard(board);
    if(sudokuSolver.readBoardFromFiles()){
        clock_t a,b;
        a=clock();
        //sudokuSolver.inferAC3();
        //sudokuSolver.inferAC3Improved();
        sudokuSolver.inferAC3Guessing();
        b=clock();
        sudokuSolver.printStatus();
        std::cout << "Puzzle solved. Time took: " << double(b)/CLOCKS_PER_SEC << "s" << std::endl;
    }
    sudokuSolver.endMessage();

    return 0;
}
