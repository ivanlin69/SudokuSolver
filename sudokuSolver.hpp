#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>

// Hash function for std::pair<int, int>, used in unordered maps and sets
struct pairHashing{
    std::size_t operator()(const std::pair<int, int>& pair) const {
        std::size_t hash1 = std::hash<int>{}(pair.first);
        std::size_t hash2 = std::hash<int>{}(pair.second);
        // Combine the hashes by shifting and XOR
        return hash1 ^ (hash2 << 1);
    }
};

// Hash function for std::pair<std::pair<int, int>, std::pair<int, int>>
struct pairOfPairHashing{
    std::size_t operator()(const std::pair<std::pair<int, int>, std::pair<int, int>>& pairOfPair) const {
        std::size_t hash1 = std::hash<int>{}(pairOfPair.first.first);
        std::size_t hash2 = std::hash<int>{}(pairOfPair.first.second);
        // Combine the hashes by shifting and XOR
        return hash1 ^ (hash2 << 1);
    }
};

// Comparator for priority queue where pairs are sorted by the first element
struct cellComparator{
    // overloading the function call operator '()' for a class/struct.
    // '()' operator allows the instances of the class/struct to be called as if they were functions.
    bool operator()(const std::pair<int, std::pair<int, int>>& a, const std::pair<int, std::pair<int, int>>& b) const{
        return a.first > b.first;
    }
};


class SudokuSolver {

private:
    // Set of all possible cell indices in the Sudoku grid
    std::set<std::pair<int, int>> cellIndices;
    // Map of each cell index to its possible values
    std::unordered_map<std::pair<int, int>, std::unordered_set<int>, pairHashing> cellValues;
    // Maps for neighbors: Row, Column, and Block
    std::unordered_map<std::pair<int, int>, std::unordered_set<std::pair<int, int>, pairHashing>, pairHashing> rowNeighbors;
    std::unordered_map<std::pair<int, int>, std::unordered_set<std::pair<int, int>, pairHashing>, pairHashing> colNeighbors;
    std::unordered_map<std::pair<int, int>, std::unordered_set<std::pair<int, int>, pairHashing>, pairHashing> blockNeighbors;
    // Map of all neighbors combined
    std::unordered_map<std::pair<int, int>, std::unordered_set<std::pair<int, int>, pairHashing>, pairHashing> allNeighbors;
    // Set of all arcs used for solving algorithms
    std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>, pairOfPairHashing> arcs;
    // Private member functions for internal calculations
    std::unordered_set<std::pair<int, int>, pairHashing> getRowNeighbors(const std::pair<int, int>& target) const;
    std::unordered_set<std::pair<int, int>, pairHashing> getColNeighbors(const std::pair<int, int>& target) const;
    std::unordered_set<std::pair<int, int>, pairHashing> getBlockNeighbors(const std::pair<int, int>& target) const;

    std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>, pairOfPairHashing> getArcs();

    // Check if the Sudoku is solved
    bool isSolved() const;
    // Enforce consistency between pairs
    // Note this function works regardless of arcs
    bool removeInconsistentValue(const std::pair<int, int>& index1, const std::pair<int, int>& index2);
    // Validate candidate numbers
    bool isCandidateValid(int candidate, const std::unordered_set<std::pair<int, int>, pairHashing>& neighbors);


public:
    SudokuSolver();
    // Initialize board from string
    void readBoard(const std::string& board);
    // Initialize board from file
    bool readBoardFromFiles();
    // Apply AC3 algorithm
    void inferAC3();
    // Apply improved version of AC3(Enhanced Constraint Propagation)
    void inferAC3Improved();
    // Apply AC3 with guessing enhancement(Backtracking)
    bool inferAC3Guessing();
    // Print the current status of the board
    void printStatus();
    // Display a welcome message
    void welcomeMessage();
    // Display a closing message
    void endMessage();
    virtual ~SudokuSolver() = default;
};

// Overloads the output stream operator to print the contents of an unordered set of different types of elements.
std::ostream& operator<<(std::ostream& os, const std::unordered_set<int>& set);
std::ostream& operator<<(std::ostream& os, const std::unordered_set<std::pair<int, int>>& set);
std::ostream& operator<<(std::ostream& os, const std::unordered_set<std::pair<std::pair<int, int>, std::pair<int, int>>>& set);
