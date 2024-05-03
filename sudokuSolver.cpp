#include "sudokuSolver.hpp"
#include <iostream>

SudokuSolver::SudokuSolver() {
    for(int i=1; i<10; i++){
        for(int j=1; j<10; j++){
            std::pair<int, int> index = std::make_pair(i, j);
            cellIndices.insert(index);
            std::set<int> values = {1,2,3,4,5,6,7,8,9};
            cells[index] = values;
        }
    }
}

void SudokuSolver::readBoard(std::string board) {
    int i = 0;
    for (std::map<std::pair<int, int>, std::set<int>>::iterator it = cells.begin(); it != cells.end(); ++it) {
        int value = board[i] - '0';
        if(value != 0){
            it->second = {value};
        }
        i++;
    }
}

std::set<std::pair<int, int>> SudokuSolver::getRowNeighbors(std::pair<int, int> target){

    std::set<std::pair<int, int>> rowNeighbors;

    for(int i=1; i<10; i++){
        rowNeighbors.insert(std::make_pair(i, target.second));
    }
    rowNeighbors.erase(target);
    return rowNeighbors;
}

std::set<std::pair<int, int>> SudokuSolver::getColumnNeighbors(std::pair<int, int> target){

    std::set<std::pair<int, int>> columnNeighbors;

    for(int i=1; i<10; i++){
        columnNeighbors.insert(std::make_pair(target.first, i));
    }
    columnNeighbors.erase(target);
    return columnNeighbors;
}

std::set<std::pair<int, int>> SudokuSolver::getBlockNeighbors(std::pair<int, int> target){

    std::set<std::pair<int, int>> blockNeighbors;
    int row = (target.first - 1) - (target.first - 1) % 3;
    int column = (target.second - 1) - (target.second - 1) % 3;

    for(int i=1; i<4; i++){
        for(int j=1; j<4; j++){
            blockNeighbors.insert(std::make_pair(row + i, column + j));
        }
    }
    blockNeighbors.erase(target);
    return blockNeighbors;
}

std::set<std::pair<int, int>> SudokuSolver::getAllNeighbors(std::pair<int, int> target){

    std::set<std::pair<int, int>> neighbors;
    std::set<std::pair<int, int>> rowNeighbors = getRowNeighbors(target);
    std::set<std::pair<int, int>> columnNeighbors = getColumnNeighbors(target);
    std::set<std::pair<int, int>> blockNeighbors = getBlockNeighbors(target);

    neighbors.insert(rowNeighbors.begin(), rowNeighbors.end());
    neighbors.insert(columnNeighbors.begin(), columnNeighbors.end());
    neighbors.insert(blockNeighbors.begin(), blockNeighbors.end());

    return neighbors;
}

std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> SudokuSolver::getArcs() {
    std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> arcs;

    for(std::set<std::pair<int, int>>::iterator it=cellIndices.begin(); it!= cellIndices.end(); ++it){
        std::pair<int, int> target = *it;

        std::set<std::pair<int, int>> neighbors = getAllNeighbors(target);

        for(std::set<std::pair<int, int>>::iterator it2 = neighbors.begin(); it2 != neighbors.end(); ++it2){
            std::pair<std::pair<int, int>, std::pair<int, int>> arc = std::make_pair(target, *it2);
            arcs.insert(arc);
        }
    }
    return arcs;
}

bool SudokuSolver::isSolved(){
    for(std::pair<int, int> index : cellIndices){
        if(cells[index].size() != 1){
            return false;
        }
    }
    return true;
}

bool SudokuSolver::removeInconsistentValue(std::pair<int, int> index1, std::pair<int, int> index2){
    bool removed = false;
    if(cells[index2].size() == 1){
        int target = *cells[index2].begin();
        std::set<int>::iterator it=cells[index1].begin();
        while(it!= cells[index1].end()){
            if(*it == target){
                // .erase return an iterator pointing to the element that follows the element last removed (or vector::end() if the last element was removed)
                cells[index1].erase(it);
                removed = true;
                break;
            }
            ++it;
        }
    }
    return removed;
}

void SudokuSolver::inferAC3(){

    std::queue<std::pair<std::pair<int, int>, std::pair<int, int>>> queueArc;
    std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> arcs = getArcs();

    std::set<std::pair<std::pair<int, int>, std::pair<int, int>>>::iterator it = arcs.begin();

    while(it!= arcs.end()){
        queueArc.push(*it);
        ++it;
    }

    while(!queueArc.empty()){
        std::pair<std::pair<int, int>, std::pair<int, int>> targetArc = queueArc.front();
        queueArc.pop();

        if(removeInconsistentValue(targetArc.first, targetArc.second)){
            std::set<std::pair<int, int>> candidates = getAllNeighbors(targetArc.first);
            candidates.erase(targetArc.second);

            std::set<std::pair<int, int>>::iterator it2 = candidates.begin();

            while(it2 != candidates.end()){
                queueArc.push(std::make_pair(*it2, targetArc.first));
                ++it2;
            }
        }
    }
}

bool SudokuSolver::isCandidateValid(int candidate, std::set<std::pair<int, int>> neighbors){

    for(std::pair<int, int> pair : neighbors){
        std::set<int> values = getValues(pair);
        if(values.find(candidate) != values.end()){
            return false;
        }
    }
    return true;
}

void SudokuSolver::inferAC3Improved(){
    inferAC3();
    int attempts = 0;

    while(!isSolved()){
        ++attempts;

        std::map<std::pair<int, int>, std::set<int>> prevCells(cells);
        std::map<std::pair<int, int>, std::set<int>>::iterator it = cells.begin();

        while(it != cells.end()){
            if((it->second).size() > 1){
                std::set<int> candidates = it->second;
                for(int candidate : candidates){
                    if(isCandidateValid(candidate, getRowNeighbors(it->first)) || isCandidateValid(candidate, getColumnNeighbors(it->first)) || isCandidateValid(candidate, getBlockNeighbors(it->first))){
                        cells[it->first] = {candidate};
                        break;
                    }
                }
            }
            inferAC3();
            ++it;
        }
        if(prevCells == cells){
            break;
        }
        std::cout << "# of attempts: " << attempts << "\n";
    }
}

// backtracking
bool SudokuSolver::inferAC3Guessing(){

    inferAC3Improved();

    if(isSolved()){
        return true;
    }
    for(std::pair<std::pair<int, int>, std::set<int>> cell: cells){
        if(cell.second.size() < 1){
            return false;
        }
    }

    class cellComparator{
    public:
        // overloading the function call operator '()' for a class/struct.
        // '()' operator allows the instances of the class/struct to be called as if they were functions.
        bool operator()(const std::pair<int, std::pair<int, int>>& a, const std::pair<int, std::pair<int, int>>& b) const{
            return a.first > b.first;
        }
    };

    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, cellComparator> minHeap;

    for(const std::pair<int, int>& index : cellIndices){
        if(cells[index].size() > 1){
            minHeap.push({cells[index].size(), index});
        }
    }

    if(minHeap.empty()){
        return isSolved();
    }

    std::pair<int, int> targetCell = minHeap.top().second;
    minHeap.pop();
    std::set<int> candidates = cells[targetCell];
    for(int candidate : candidates){
        std::map<std::pair<int, int>, std::set<int>> prevCells = cells;
        // start guessing
        cells[targetCell] = {candidate};

        if(inferAC3Guessing()){
            return true;
        }
        else{
            cells = prevCells;
        }
    }
    return false;
}

void SudokuSolver::printStatus(){

    std::map<std::pair<int, int>, std::set<int>>::iterator it = cells.begin();
    int i = 1;
    while(it != cells.end()){
        std::cout << it->second << " ";
        if(i == 9){
            std::cout << "\n";
            i= 0;
        }
        ++i;
        ++it;
    }
}

std::ostream& operator<<(std::ostream& os, const std::vector<int>& vec) {
    for (const int& value : vec) {
        os << value << " ";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::set<int>& set) {
    os << "{";
    for (const int& value : set) {
        os << value << " ";
    }
    os << "} ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::set<std::pair<int, int>>& set) {
    os << "{";
    for (const std::pair<int, int>& pair : set) {
        os << "(" << pair.first << ", " << pair.second << "), \n";
    }
    os << "} ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::set<std::pair<std::pair<int, int>, std::pair<int, int>>>& set) {
    os << "{";
    for (const std::pair<std::pair<int, int>, std::pair<int, int>>& pair : set) {
        os << "((" << pair.first.first << ", " << pair.first.second << "), (" << pair.second.first << ", " << pair.second.second << ")), \n";
    }
    os << "} ";
    return os;
}
