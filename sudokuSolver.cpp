#include "sudokuSolver.hpp"
#include <iostream>
#include <fstream>

SudokuSolver::SudokuSolver() {
    // Initialize each cell with possible values from 1 to 9
    const std::unordered_set<int> values = {1,2,3,4,5,6,7,8,9};
    // Populate cellIndices and cellValues for each cell in a 9x9 grid
    for(int row=1; row<=9; row++){
        for(int col=1; col<=9; col++){
            std::pair<int, int> index = {row, col};
            cellIndices.emplace(index);
            cellValues.emplace(index, values);
        }
    }
    // Assign neighbors for each cell to maintain row, column, and block relationships
    for(const std::pair<int, int>& pair : cellIndices){
        rowNeighbors[pair] = getRowNeighbors(pair);
        colNeighbors[pair] = getColNeighbors(pair);
        blockNeighbors[pair] = getBlockNeighbors(pair);

        // Combine all types of neighbors into allNeighbors for easy access
        allNeighbors[pair].insert(rowNeighbors.at(pair).begin(), rowNeighbors.at(pair).end());
        allNeighbors.at(pair).insert(colNeighbors.at(pair).begin(), colNeighbors.at(pair).end());
        allNeighbors.at(pair).insert(blockNeighbors.at(pair).begin(), blockNeighbors.at(pair).end());
    }
    // Initialize arcs based on the current neighbors setup
    arcs = getArcs();
}

// Reads a Sudoku board from a string input, initializing cell values.
void SudokuSolver::readBoard(const std::string& board) {
    int i = 0;
    for(int row=1; row<=9; ++row){
        for(int col=1; col<=9; ++col){
            std::pair<int, int> index = {row, col};
            // Convert character to integer
            int value = board.at(i) - '0';
            if(value != 0){
                // Set the cell value if not zero
                cellValues.at(index) = {value};
            }
            ++i;
        }
    }
}

// Reads a Sudoku puzzle from a file input by prompting the user.
bool SudokuSolver::readBoardFromFiles(){
    std::string userInput;
    std::ifstream file;

    do{
        std::cout << "Please enter the file name(including '.txt'),\nOr enter '--exit' to leave the program:\n";
        std::getline(std::cin, userInput);

        if(userInput == "--exit"){
            file.close();
            return false;
        }

        std::cout << "\n ... loading the file ... \n";
        file.open(userInput);

        if(!file.is_open()){
            std::cerr << "\nFailed to open the file " << userInput << " . Please check the file location/name and try again. Or enter '--exit' to leave the program.\n\n";
        }
    } while(!file.is_open());

    // Read the file line by line and set the values in the Sudoku grid
    std::string line;
    int k = 0;
    int row = 1;
    while(getline(file, line) && row <= 9){
        for(int col = 1; col <=9; ++col){
            std::pair<int, int> index = {row, col};
            char ch = line.at(k++);
            // Ignore '*' as placeholder for empty cells
            if(ch != '*'){
                int value = ch - '0';
                cellValues.at(index) = {value};
            }
        }
        k=0;
        ++row;
    }
    file.close();
    std::cout << "\nFile loaded successfully. Here is your puzzle:\n";
    printStatus();

    std::cout << "Starts analyzing the puzzle ... \n";
    return true;
}

/**
 * Retrieves all row neighbors of a given cell excluding the cell itself.
 * This function collects all cells that are in the same row as the target cell.
 *
 * @param target The cell for which to find row neighbors.
 * @return An unordered set of pairs representing the indices of the row neighbors.
 */
std::unordered_set<std::pair<int, int>, pairHashing> SudokuSolver::getRowNeighbors(const std::pair<int, int>& target) const {

    std::unordered_set<std::pair<int, int>, pairHashing> rowNeighbors;
    for(int row=1; row<=9; ++row){
        rowNeighbors.emplace(row, target.second);
    }
    // Remove the target cell itself from its neighbors
    rowNeighbors.erase(target);
    return rowNeighbors;
}

/**
 * Retrieves all column neighbors of a given cell excluding the cell itself.
 * This function collects all cells that are in the same column as the target cell.
 *
 * @param target The cell for which to find column neighbors.
 * @return An unordered set of pairs representing the indices of the column neighbors.
 */
std::unordered_set<std::pair<int, int>, pairHashing> SudokuSolver::getColNeighbors(const std::pair<int, int>& target) const {

    std::unordered_set<std::pair<int, int>, pairHashing> colNeighbors;
    for(int col=1; col<=9; ++col){
        colNeighbors.emplace(target.first, col);
    }
    // Remove the target cell itself from its neighbors
    colNeighbors.erase(target);
    return colNeighbors;
}

/**
 * Retrieves all neighbors within the same 3x3 block of a given cell excluding the cell itself.
 * This calculates the top left corner of the block and iterates over the 3x3 grid.
 *
 * @param target The cell for which to find block neighbors.
 * @return An unordered set of pairs representing the indices of the block neighbors.
 */
std::unordered_set<std::pair<int, int>, pairHashing> SudokuSolver::getBlockNeighbors(const std::pair<int, int>& target) const {

    std::unordered_set<std::pair<int, int>, pairHashing> blockNeighbors;
    // Calculate starting row of the block
    int row = (target.first - 1) - (target.first - 1) % 3;
    // Calculate starting column of the block
    int column = (target.second - 1) - (target.second - 1) % 3;

    for(int i=1; i<4; i++){
        for(int j=1; j<4; j++){
            blockNeighbors.emplace(row + i, column + j);
        }
    }
    // Remove the target cell itself from its neighbors
    blockNeighbors.erase(target);
    return blockNeighbors;
}

/**
 * Builds the set of arcs needed for the AC-3 algorithm. An arc represents a pair of cells where
 * one influences the possible values of the other.
 *
 * @return A set of pairs where each pair consists of two cell indices representing an arc.
 */
std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>, pairOfPairHashing> SudokuSolver::getArcs(){

    for(const std::pair<int, int>& targetIndex : cellIndices){
        std::unordered_set<std::pair<int, int>, pairHashing> neighbors = allNeighbors.at(targetIndex);
        for(const std::pair<int, int>& neighbor : neighbors){
            arcs.emplace(targetIndex, neighbor);
        }
    }
    return arcs;
}

/**
 * Checks if the Sudoku puzzle is solved. A puzzle is considered solved if each cell
 * in the Sudoku grid has exactly one possible value (i.e., no ambiguity remains).
 *
 * @return true if the puzzle is solved, false otherwise.
 */
bool SudokuSolver::isSolved() const {
    for(const std::pair<int, int>& index : cellIndices){
        if(cellValues.at(index).size() != 1){
            return false;
        }
    }
    return true;
}

/**
 * Removes an inconsistent value from the possible values of a cell if the neighboring
 * cell has a determined value. This method enforces the constraint that no two
 * neighboring cells can contain the same value.
 *
 * @param index1 The cell to potentially modify.
 * @param index2 The neighboring cell which might influence index1.
 * @return true if a value was removed, false otherwise.
 */
bool SudokuSolver::removeInconsistentValue(const std::pair<int, int>& index1, const std::pair<int, int>& index2){
    bool removed = false;
    const std::unordered_set<int>& value2 = cellValues.at(index2);
    // Only proceed if index2 has exactly one value (determined)
    if(value2.size() == 1){
        std::unordered_set<int>& value1 = cellValues.at(index1);
        // .erase() returns 0 if the value to remove was not found, and 1 otherwise.
        if(value1.erase(*value2.begin()) == 1){
            removed = true;
        }
    }
    return removed;
}

/**
 * Applies the AC3 algorithm to reduce the domain of possible values for each cell
 * by achieving arc consistency across the Sudoku grid. If a value is removed from
 * a cell's domain, it re-evaluates its neighbors to ensure consistency is maintained.
 */
void SudokuSolver::inferAC3(){

    std::queue<std::pair<std::pair<int, int>, std::pair<int, int>>> queueArc;
    // Initialize the queue with all existing arcs
    for(const std::pair<std::pair<int, int>, std::pair<int, int>>& arc : arcs){
        queueArc.push(arc);
    }

    while(!queueArc.empty()){
        const std::pair<std::pair<int, int>, std::pair<int, int>>& targetArc = queueArc.front();
        queueArc.pop();
        // If removing an inconsistent value is successful, requeue neighbors for further consistency checks
        if(removeInconsistentValue(targetArc.first, targetArc.second)){
            std::unordered_set<std::pair<int, int>, pairHashing> candidates = allNeighbors.at(targetArc.first);
            // Remove the current arc's target to prevent reciprocal checking
            candidates.erase(targetArc.second);
            // Re-enqueue all affected arcs
            for(const std::pair<int, int>& candidate : candidates){
                queueArc.push({candidate, targetArc.first});
            }
        }
    }
}

/**
 * Validates whether a candidate number can be legally placed in a cell based on its neighbors.
 * It checks if the candidate number is already present in any of the neighboring cells' values.
 *
 * @param candidate The number to validate.
 * @param neighbors A set of neighbors' indices to check against the candidate.
 * @return true if the candidate is valid (not found in neighbors), false otherwise.
 */
bool SudokuSolver::isCandidateValid(int candidate, const std::unordered_set<std::pair<int, int>, pairHashing>& neighbors){

    for(const std::pair<int, int>& pair : neighbors){
        const std::unordered_set<int>& values = cellValues.at(pair);
        // .count() returns 0 if the element is not found, or 1 otherwise.
        if(values.count(candidate) == 1){
            return false;
        }
    }
    return true;
}

/**
 * An improved AC3 algorithm that considers additional constraints by validating each candidate
 * against all neighbors before deciding to place a value. This is used to solve more complex puzzles
 * where standard AC3 might not suffice.
 */
void SudokuSolver::inferAC3Improved(){
    inferAC3();
    int attempts = 0;

    while(!isSolved()){
        ++attempts;
        bool convergence = true;

        std::unordered_map<std::pair<int, int>, std::unordered_set<int>, pairHashing> prevCells(cellValues);
        // remember that keys are immutable
        for(const std::pair<const std::pair<int, int>, std::unordered_set<int>>& pair : cellValues){

            const std::pair<int, int>& index = pair.first;
            const std::unordered_set<int>& candidates = pair.second;

            if(candidates.size() > 1){
                for(int candidate : candidates){
                    if(isCandidateValid(candidate, rowNeighbors.at(index)) ||
                       isCandidateValid(candidate, colNeighbors.at(index)) ||
                       isCandidateValid(candidate, blockNeighbors.at(index)) ){
                        cellValues.at(index) = {candidate};
                        convergence = false;
                        break;
                    }
                }
            }
            inferAC3();
        }
        if(convergence){
            break;
        }
        std::cout << "# of attempts: " << attempts << "\n";
    }
}


/**
 * Applies guessing strategies combined with the improved AC3 algorithm to solve the Sudoku.
 * It uses a backtracking approach with a priority queue to handle cells with more constraints first.
 *
 * @return true if the puzzle is solved, false if no solution is found.
 */
bool SudokuSolver::inferAC3Guessing(){

    inferAC3Improved();

    if(isSolved()){
        return true;
    }

    for(const std::pair<const std::pair<int, int>, std::unordered_set<int>>& cell: cellValues){
        if(cell.second.empty()){
            // No valid values left to assign, means it's unsolvable
            return false;
        }
    }

    // This priority queue with cells based on the minimum remaining values heuristic,
    // which prioritizes cells with the fewest possible values left.
    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, cellComparator> minHeap;

    // Iterates through all cell indices and populate the queue
    for(const std::pair<int, int>& index : cellIndices){
        std::size_t size = cellValues.at(index).size();
        if(size > 1){
            minHeap.push({size, index});
        }
    }

    std::pair<int, int> targetCell = minHeap.top().second;
    minHeap.pop();
    std::unordered_set<int> candidates = cellValues.at(targetCell);

    for(int candidate : candidates){
        std::unordered_map<std::pair<int, int>, std::unordered_set<int>, pairHashing> prevCells = cellValues;
        // start guessing
        cellValues.at(targetCell) = {candidate};

        if(inferAC3Guessing()){
            return true;
        }
        else{
            cellValues = prevCells;
        }
    }
    return false;
}

/**
 * Prints the current status of the Sudoku grid. Cells with multiple possibilities are printed with their possible values.
 * Empty cells (with all possibilities) are printed as blank.
 */
void SudokuSolver::printStatus(){
    std::cout << std::endl;

    for(const std::pair<int, int>& index : cellIndices){
        const std::unordered_set<int>& values = cellValues.at(index);
        if(values.size() != 9){
            std::cout << cellValues.at(index) << " ";
        } else {
            std::cout << "{ } ";
        }
        if(index.second == 9){
            std::cout << "\n";
        }
    }
    std::cout << std::endl;
}

void SudokuSolver::welcomeMessage() {
    std::cout << "\n\n";
    std::cout << "================================================================================\n";
    std::cout << " _________         .___      __          _________      .__             -v1.8.1\n";
    std::cout << "/   _____/__ __  __| _/____ |  | ____ __/   _____/ ____ |  |___  __ ___________\n";
    std::cout << "\\_____  \\|  |  \\/ __ |/  _ \\|  |/ /  |  \\_____  \\ /  _ \\|  |\\  \\/ // __ \\_  __ \\\n";
    std::cout << "/        \\  |  / /_/ (  <_> )    <|  |  /        (  <_> )  |_\\   /\\  ___/|  | \\/\n";
    std::cout << "/_______ /____/\\_____|\\____/|__|_ \\____/_______  /\\____/|____/\\_/  \\___  >__|\n";
    std::cout << "       \\/           \\/           \\/            \\/                      \\/\n";
    std::cout << "================================================================================\n";
    std::cout << "Welcome to Sudoku solver! Let me solve the sudoku puzzle for you ... \n";
    std::cout << "First, I need your help to load the puzzle file.\n\n";
}

void SudokuSolver::endMessage() {
  std::cout << "\nThank you and good bye!\n\n";
    std::cout << "================================================================================\n";
}

/**
 * Overloads the output stream operator to print the contents of an unordered set of integers.
 * Formats the output by enclosing the integers in curly braces and listing them without commas for simplicity.
 */
std::ostream& operator<<(std::ostream& os, const std::unordered_set<int>& set) {
    os << "{";
    for (const int& value : set) {
        os << value;
    }
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::unordered_set<std::pair<int, int>>& set) {
    os << "{";
    for (const std::pair<int, int>& pair : set) {
        os << "(" << pair.first << ", " << pair.second << "), \n";
    }
    os << "}";
    return os;
}

/**
 * Overloads the output stream operator to print the contents of an unordered set of pair of pairs(used for displaying arcs if needed)
 * Formats the output by enclosing the pairs in curly braces and listing them without commas for simplicity.
 */

std::ostream& operator<<(std::ostream& os, const std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>>& set) {
    os << "{";
    for (const std::pair<std::pair<int, int>, std::pair<int, int>>& pair : set) {
        os << "((" << pair.first.first << ", " << pair.first.second << "), (" << pair.second.first << ", " << pair.second.second << ")), \n";
    }
    os << "} ";
    return os;
}
