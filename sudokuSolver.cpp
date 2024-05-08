#include "sudokuSolver.hpp"
#include <iostream>

SudokuSolver::SudokuSolver() {

    const std::unordered_set<int> values = {1,2,3,4,5,6,7,8,9};

    for(int row=1; row<=9; row++){
        for(int col=1; col<=9; col++){
            std::pair<int, int> index = {row, col};
            cellIndices.emplace(index);
            cellValues.emplace(index, values);
        }
    }

    for(const std::pair<int, int>& pair : cellIndices){
        rowNeighbors[pair] = getRowNeighbors(pair);
        colNeighbors[pair] = getColNeighbors(pair);
        blockNeighbors[pair] = getBlockNeighbors(pair);

        allNeighbors[pair].insert(rowNeighbors.at(pair).begin(), rowNeighbors.at(pair).end());
        allNeighbors.at(pair).insert(colNeighbors.at(pair).begin(), colNeighbors.at(pair).end());
        allNeighbors.at(pair).insert(blockNeighbors.at(pair).begin(), blockNeighbors.at(pair).end());
    }

    arcs = getArcs();
}

void SudokuSolver::readBoard(const std::string& board) {
    int k = 0;
    for(int row=1; row<=9; ++row){
        for(int col=1; col<=9; ++col){
            std::pair<int, int> index = {row, col};
            int value = board.at(k) - '0';
            if(value != 0){
                cellValues.at(index) = {value};
            }
            k++;
        }
    }
}

std::unordered_set<std::pair<int, int>, pairHashing> SudokuSolver::getRowNeighbors(const std::pair<int, int>& target) const {

    std::unordered_set<std::pair<int, int>, pairHashing> rowNeighbors;
    for(int row=1; row<=9; ++row){
        rowNeighbors.emplace(row, target.second);
    }
    rowNeighbors.erase(target);
    return rowNeighbors;
}

std::unordered_set<std::pair<int, int>, pairHashing> SudokuSolver::getColNeighbors(const std::pair<int, int>& target) const {

    std::unordered_set<std::pair<int, int>, pairHashing> colNeighbors;
    for(int col=1; col<=9; ++col){
        colNeighbors.emplace(target.first, col);
    }
    colNeighbors.erase(target);
    return colNeighbors;
}

std::unordered_set<std::pair<int, int>, pairHashing> SudokuSolver::getBlockNeighbors(const std::pair<int, int>& target) const {

    std::unordered_set<std::pair<int, int>, pairHashing> blockNeighbors;
    int row = (target.first - 1) - (target.first - 1) % 3;
    int column = (target.second - 1) - (target.second - 1) % 3;

    for(int i=1; i<4; i++){
        for(int j=1; j<4; j++){
            blockNeighbors.emplace(row + i, column + j);
        }
    }
    blockNeighbors.erase(target);
    return blockNeighbors;
}


std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>, pairOfPairHashing> SudokuSolver::getArcs(){

    for(const std::pair<int, int>& targetIndex : cellIndices){
        std::unordered_set<std::pair<int, int>, pairHashing> neighbors = allNeighbors.at(targetIndex);
        for(const std::pair<int, int>& neighbor : neighbors){
            arcs.emplace(targetIndex, neighbor);
        }
    }
    return arcs;
}

bool SudokuSolver::isSolved() const {
    for(const std::pair<int, int>& index : cellIndices){
        if(cellValues.at(index).size() != 1){
            return false;
        }
    }
    return true;
}

bool SudokuSolver::removeInconsistentValue(const std::pair<int, int>& index1, const std::pair<int, int>& index2){
    bool removed = false;
    const std::unordered_set<int>& value2 = cellValues.at(index2);
    if(value2.size() == 1){
        std::unordered_set<int>& value1 = cellValues.at(index1);
        // .erase() returns 0 if the value to remove was not found, and 1 otherwise.
        if(value1.erase(*value2.begin()) == 1){
            removed = true;
        }
    }
    return removed;
}

void SudokuSolver::inferAC3(){

    std::queue<std::pair<std::pair<int, int>, std::pair<int, int>>> queueArc;

    for(const std::pair<std::pair<int, int>, std::pair<int, int>>& arc : arcs){
        queueArc.push(arc);
    }

    while(!queueArc.empty()){
        const std::pair<std::pair<int, int>, std::pair<int, int>>& targetArc = queueArc.front();
        queueArc.pop();

        if(removeInconsistentValue(targetArc.first, targetArc.second)){
            std::unordered_set<std::pair<int, int>, pairHashing> candidates = allNeighbors.at(targetArc.first);
            candidates.erase(targetArc.second);

            for(const std::pair<int, int>& candidate : candidates){
                queueArc.push({candidate, targetArc.first});
            }
        }
    }
}

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


// backtracking
bool SudokuSolver::inferAC3Guessing(){

    inferAC3Improved();

    if(isSolved()){
        return true;
    }

    for(const std::pair<const std::pair<int, int>, std::unordered_set<int>>& cell: cellValues){
        if(cell.second.empty()){
            return false;
        }
    }

    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, cellComparator> minHeap;

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

void SudokuSolver::printStatus(){
    for(const std::pair<int, int>& index : cellIndices){
        std::cout << cellValues.at(index) << " ";
            if(index.second == 9){
                std::cout << "\n";
            }
    }
}

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

std::ostream& operator<<(std::ostream& os, const std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>>& set) {
    os << "{";
    for (const std::pair<std::pair<int, int>, std::pair<int, int>>& pair : set) {
        os << "((" << pair.first.first << ", " << pair.first.second << "), (" << pair.second.first << ", " << pair.second.second << ")), \n";
    }
    os << "} ";
    return os;
}
