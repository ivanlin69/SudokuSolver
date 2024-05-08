#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>

struct pairHashing{
    std::size_t operator()(const std::pair<int, int>& pair) const {
        std::size_t hash1 = std::hash<int>{}(pair.first);
        std::size_t hash2 = std::hash<int>{}(pair.second);
        return hash1 ^ (hash2 << 1);
    }
};

struct pairOfPairHashing{
    std::size_t operator()(const std::pair<std::pair<int, int>, std::pair<int, int>>& pairOfPair) const {
        std::size_t hash1 = std::hash<int>{}(pairOfPair.first.first);
        std::size_t hash2 = std::hash<int>{}(pairOfPair.first.second);
        return hash1 ^ (hash2 << 1);
    }
};

struct cellComparator{
    // overloading the function call operator '()' for a class/struct.
    // '()' operator allows the instances of the class/struct to be called as if they were functions.
    bool operator()(const std::pair<int, std::pair<int, int>>& a, const std::pair<int, std::pair<int, int>>& b) const{
        return a.first > b.first;
    }
};


class SudokuSolver {

private:
    std::set<std::pair<int, int>> cellIndices;
    std::unordered_map<std::pair<int, int>, std::unordered_set<int>, pairHashing> cellValues;
    std::unordered_map<std::pair<int, int>, std::unordered_set<std::pair<int, int>, pairHashing>, pairHashing> rowNeighbors;
    std::unordered_map<std::pair<int, int>, std::unordered_set<std::pair<int, int>, pairHashing>, pairHashing> colNeighbors;
    std::unordered_map<std::pair<int, int>, std::unordered_set<std::pair<int, int>, pairHashing>, pairHashing> blockNeighbors;
    std::unordered_map<std::pair<int, int>, std::unordered_set<std::pair<int, int>, pairHashing>, pairHashing> allNeighbors;

    std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>, pairOfPairHashing> arcs;

    std::unordered_set<std::pair<int, int>, pairHashing> getRowNeighbors(const std::pair<int, int>& target) const;
    std::unordered_set<std::pair<int, int>, pairHashing> getColNeighbors(const std::pair<int, int>& target) const;
    std::unordered_set<std::pair<int, int>, pairHashing> getBlockNeighbors(const std::pair<int, int>& target) const;

    std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>, pairOfPairHashing> getArcs();

    bool isSolved() const;
    // Note this function works regardless of arcs
    bool removeInconsistentValue(const std::pair<int, int>& index1, const std::pair<int, int>& index2);
    bool isCandidateValid(int candidate, const std::unordered_set<std::pair<int, int>, pairHashing>& neighbors);


public:
    SudokuSolver();

    void readBoard(const std::string& board);
    void inferAC3();
    void inferAC3Improved();
    bool inferAC3Guessing();
    void printStatus();

    virtual ~SudokuSolver() = default;
};

std::ostream& operator<<(std::ostream& os, const std::unordered_set<int>& set);
std::ostream& operator<<(std::ostream& os, const std::unordered_set<std::pair<int, int>>& set);
std::ostream& operator<<(std::ostream& os, const std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>>& set);
