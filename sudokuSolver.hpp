#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>
#include <functional>
#include <utility>

struct pairHashing{
    std::size_t operator()(const std::pair<int, int>& pair) const noexcept{
        auto hash1 = std::hash<int>{}(pair.first);
        auto hash2 = std::hash<int>{}(pair.second);
        return hash1 ^ (hash2 << 1);
    }
};

struct pairOfPairHashing{
    std::size_t operator()(const std::pair<std::pair<int, int>, std::pair<int, int>>& pairOfPair) const noexcept{
        auto hash1 = std::hash<int>{}(pairOfPair.first.first);
        auto hash2 = std::hash<int>{}(pairOfPair.first.second);
        return hash1 ^ (hash2 << 1);
    }
};

class SudokuSolver {

private:
    std::unordered_set<std::pair<int, int>, pairHashing> cellIndices;
    std::unordered_map<std::pair<int, int>, std::unordered_set<int>, pairHashing> cellValues;
    std::unordered_map<std::pair<int, int>, std::unordered_set<std::pair<int, int>, pairHashing>, pairHashing> rowNeighbors;
    std::unordered_map<std::pair<int, int>, std::unordered_set<std::pair<int, int>, pairHashing>, pairHashing> columnNeighbors;
    std::unordered_map<std::pair<int, int>, std::unordered_set<std::pair<int, int>, pairHashing>, pairHashing> blockNeighbors;
    std::unordered_map<std::pair<int, int>, std::unordered_set<std::pair<int, int>, pairHashing>, pairHashing> allNeighbors;

    std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>, pairOfPairHashing> arcs;

public:
    SudokuSolver();

    void readBoard(std::string board);

    std::unordered_set<std::pair<int, int>, pairHashing> getRowNeighbors(std::pair<int, int> target);
    std::unordered_set<std::pair<int, int>, pairHashing> getColumnNeighbors(std::pair<int, int> target);
    std::unordered_set<std::pair<int, int>, pairHashing> getBlockNeighbors(std::pair<int, int> target);

    std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>, pairOfPairHashing> getArcs();
    bool isSolved();
    // Note this function works regardless of arcs
    bool removeInconsistentValue(std::pair<int, int> index1, std::pair<int, int> index2);

    void inferAC3();
    bool isCandidateValid(int candidate, std::unordered_set<std::pair<int, int>, pairHashing> neighbors);
    void inferAC3Improved();
    bool inferAC3Guessing();
    void printStatus();

    virtual ~SudokuSolver() = default;
};

std::ostream& operator<<(std::ostream& os, const std::vector<int>& vec);
std::ostream& operator<<(std::ostream& os, const std::unordered_set<int>& set);
std::ostream& operator<<(std::ostream& os, const std::unordered_set<std::pair<int, int>>& set);
std::ostream& operator<<(std::ostream& os, const std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>>& set);
