#include "sudokuSolver.hpp"
#include <iostream>
#include "time.h"

int main() {
    
    SudokuSolver sudokuSolver;
    sudokuSolver.welcomeMessage();
    if(sudokuSolver.readBoardFromFiles()){
        
        clock_t a,b;
        a=clock();
        sudokuSolver.inferAC3Guessing();
        b=clock();
        
        sudokuSolver.printStatus();
        std::cout << "Puzzle solved. Time took: " << double(b)/CLOCKS_PER_SEC << "s" << std::endl;
    }
    sudokuSolver.endMessage();

    return 0;
}
