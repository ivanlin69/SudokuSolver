#include "sudokuSolver.hpp"
#include <iostream>

SudokuSolver::SudokuSolver() {
    for(int i=1; i<10; i++){
        for(int j=1; j<10; j++){
            cellIndices.insert({i, j});
            std::unordered_set<int> values = {1,2,3,4,5,6,7,8,9};
            cellValues[{i, j}] = values;
        }
    }

    for(std::pair<int, int> pair : cellIndices){
        rowNeighbors[pair] = getRowNeighbors(pair);
        columnNeighbors[pair] = getColumnNeighbors(pair);
        blockNeighbors[pair] = getBlockNeighbors(pair);

        allNeighbors[pair].insert(rowNeighbors.at(pair).begin(), rowNeighbors.at(pair).end());
        allNeighbors.at(pair).insert(columnNeighbors.at(pair).begin(), columnNeighbors.at(pair).end());
        allNeighbors.at(pair).insert(blockNeighbors.at(pair).begin(), blockNeighbors.at(pair).end());
    }

    arcs = getArcs();
}

void SudokuSolver::readBoard(std::string board) {
    int k = 0;
    for(int i=1; i<10; i++){
        for(int j=1; j<10; j++){
            std::pair<int, int> index = std::make_pair(i, j);
            int value = board.at(k) - '0';
            if(value != 0){
                cellValues.at(index) = {value};
            }
            k++;
        }
    }
}

std::unordered_set<std::pair<int, int>, pairHashing> SudokuSolver::getRowNeighbors(std::pair<int, int> target){

    std::unordered_set<std::pair<int, int>, pairHashing> rowNeighbors;

    for(int i=1; i<10; i++){
        rowNeighbors.insert(std::make_pair(i, target.second));
    }
    rowNeighbors.erase(target);
    return rowNeighbors;
}

std::unordered_set<std::pair<int, int>, pairHashing> SudokuSolver::getColumnNeighbors(std::pair<int, int> target){

    std::unordered_set<std::pair<int, int>, pairHashing> columnNeighbors;

    for(int i=1; i<10; i++){
        columnNeighbors.insert(std::make_pair(target.first, i));
    }
    columnNeighbors.erase(target);
    return columnNeighbors;
}

std::unordered_set<std::pair<int, int>, pairHashing> SudokuSolver::getBlockNeighbors(std::pair<int, int> target){

    std::unordered_set<std::pair<int, int>, pairHashing> blockNeighbors;
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


std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>, pairOfPairHashing> SudokuSolver::getArcs() {

    for(std::set<std::pair<int, int>>::iterator it=cellIndices.begin(); it!= cellIndices.end(); ++it){
        std::pair<int, int> target = *it;

        std::unordered_set<std::pair<int, int>, pairHashing> neighbors = allNeighbors.at(target);

        for(std::unordered_set<std::pair<int, int>>::iterator it2 = neighbors.begin(); it2 != neighbors.end(); ++it2){
            std::pair<std::pair<int, int>, std::pair<int, int>> arc = std::make_pair(target, *it2);
            arcs.insert(arc);
        }
    }
    return arcs;
}

bool SudokuSolver::isSolved() const {
    for(std::pair<int, int> index : cellIndices){
        if(cellValues.at(index).size() != 1){
            return false;
        }
    }
    return true;
}

bool SudokuSolver::removeInconsistentValue(std::pair<int, int> index1, std::pair<int, int> index2){
    bool removed = false;
    if(cellValues.at(index2).size() == 1){
        int target = *cellValues.at(index2).begin();

        std::unordered_set<int>::iterator it = cellValues.at(index1).find(target);
        if(it != cellValues.at(index1).end()){
            cellValues.at(index1).erase(it);
            removed = true;
        }
    }
    return removed;
}

void SudokuSolver::inferAC3(){

    std::queue<std::pair<std::pair<int, int>, std::pair<int, int>>> queueArc;

    std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>>::iterator it = arcs.begin();
    while(it!= arcs.end()){
        queueArc.push(*it);
        ++it;
    }

    while(!queueArc.empty()){
        std::pair<std::pair<int, int>, std::pair<int, int>> targetArc = queueArc.front();
        queueArc.pop();

        if(removeInconsistentValue(targetArc.first, targetArc.second)){
            std::unordered_set<std::pair<int, int>, pairHashing> candidates = allNeighbors.at(targetArc.first);
            candidates.erase(targetArc.second);

            std::unordered_set<std::pair<int, int>, pairHashing>::iterator it2 = candidates.begin();
            while(it2 != candidates.end()){
                queueArc.push(std::make_pair(*it2, targetArc.first));
                ++it2;
            }
        }
    }
}

bool SudokuSolver::isCandidateValid(int candidate, std::unordered_set<std::pair<int, int>, pairHashing> neighbors){

    for(std::pair<int, int> pair : neighbors){
        std::unordered_set<int> values = cellValues.at(pair);
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
        bool convergence = true;

        std::unordered_map<std::pair<int, int>, std::unordered_set<int>, pairHashing> prevCells(cellValues);
        std::unordered_map<std::pair<int, int>, std::unordered_set<int>>::iterator it = cellValues.begin();

        while(it != cellValues.end()){
            if((it->second).size() > 1){
                std::unordered_set<int> candidates = it->second;
                for(int candidate : candidates){
                    if(isCandidateValid(candidate, rowNeighbors.at(it->first)) || isCandidateValid(candidate, columnNeighbors.at(it->first)) || isCandidateValid(candidate, blockNeighbors.at(it->first))){
                        cellValues.at(it->first) = {candidate};
                        convergence = false;
                        break;
                    }
                }
            }
            inferAC3();
            ++it;
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
    for(std::pair<std::pair<int, int>, std::unordered_set<int>> cell: cellValues){
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
        if(cellValues.at(index).size() > 1){
            minHeap.push({cellValues.at(index).size(), index});
        }
    }

    if(minHeap.empty()){
        return isSolved();
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

    for(std::pair<int, int> index : cellIndices){
        std::cout << cellValues.at(index) << " ";
            if(index.second == 9){
                std::cout << "\n";
            }
    }
}

std::ostream& operator<<(std::ostream& os, const std::unordered_set<int>& set) {
    os << "{";
    for (const int& value : set) {
        os << value << " ";
    }
    os << "} ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::unordered_set<std::pair<int, int>>& set) {
    os << "{";
    for (const std::pair<int, int>& pair : set) {
        os << "(" << pair.first << ", " << pair.second << "), \n";
    }
    os << "} ";
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
