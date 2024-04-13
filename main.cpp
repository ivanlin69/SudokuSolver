#include "sudokuSolver.hpp"
#include <iostream>

int main() {

  std::string board = "82100000700080006006093000500820160000070028424060370060"
                      "5000103070050000912000006";

  SudokuSolver sudokuSolver;
  sudokuSolver.readBoard(board);

  /*
  std::pair<int, int> index = std::make_pair(1, 1);
  std::cout << "Test getValues(1,1): " << sudokuSolver.getValues(index) << "\n";
  std::cout << "Test getCellIndices(): " <<  sudokuSolver.getCellIndices() <<
  "\n";

  std::set<std::pair<int, int>> neighbors =
  sudokuSolver.getAllNeighbors(std::make_pair(2, 3)); std::cout << "Test
  getAllNeighbors - (2,3): " << sudokuSolver.getAllNeighbors(std::make_pair(2,
  3)) << "\n";

  std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> arcs =
  sudokuSolver.getArcs();

  std::cout << "Test arcs: " << arcs;

  std::pair<std::pair<int, int>, std::pair<int, int>> target =
  std::pair(std::make_pair(3, 4), std::make_pair(1, 1));

  auto it = std::find(arcs.begin(), arcs.end(), target);
  if(it == arcs.end()){
   std::cout << "Test arcs: (3,4) and (1,1) - " << "Not found.\n";
  } else{
   std::cout << "Test arcs: (3,4) and (1,1) - " << it->second.second <<
  "Found!\n";
  }

  std::cout << "Test removeInconsistentValue: (1,1) Before: " <<
  sudokuSolver.getValues(index) << "\n"; bool r =
  sudokuSolver.removeInconsistentValue(std::make_pair(1, 1), std::make_pair(2,
  1)); std::cout << std::boolalpha << "Test removeInconsistentValue: (1,1) After
  (2,1): " << sudokuSolver.getValues(index) << "Bool result:" << r << "\n";
   */
  sudokuSolver.inferAC3();
  sudokuSolver.printStatus();

  // Test Map
  // 006201000
  // 800000071
  // 001700032
  // 007030040
  // 050000080
  // 080040700
  // 460005800
  // 170000004
  // 000406500

  return 0;
}
