#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

class SudokuSolver {
private:
  std::set<std::pair<int, int>> cellIndices;
  std::map<std::pair<int, int>, std::set<int>> cells;

public:
  SudokuSolver();

  const std::set<std::pair<int, int>> getCellIndices() { return cellIndices; };
  std::set<int> getValues(std::pair<int, int> index) { return cells[index]; };
  void readBoard(std::string board);

  std::set<std::pair<int, int>> getRowNeighbors(std::pair<int, int> target);
  std::set<std::pair<int, int>> getColumnNeighbors(std::pair<int, int> target);
  std::set<std::pair<int, int>> getBlockNeighbors(std::pair<int, int> target);
  std::set<std::pair<int, int>> getAllNeighbors(std::pair<int, int> target);

  std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> getArcs();

  // Note this function works regardless of arcs
  bool removeInconsistentValue(std::pair<int, int> index1,
                               std::pair<int, int> index2);

  void inferAC3();
  void printStatus();

  virtual ~SudokuSolver() = default;
};

std::ostream &operator<<(std::ostream &os, const std::vector<int> &vec);
std::ostream &operator<<(std::ostream &os, const std::set<int> &set);
std::ostream &operator<<(std::ostream &os,
                         const std::set<std::pair<int, int>> &set);
std::ostream &operator<<(
    std::ostream &os,
    const std::set<std::pair<std::pair<int, int>, std::pair<int, int>>> &set);
