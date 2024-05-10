# Sudoku Auto Solver

## Description
This project is a Sudoku Solver that automates the process of solving Sudoku puzzles using advanced algorithmic techniques. It includes implementations of the AC-3 algorithm and uses backtracking for puzzles that cannot be solved through constraint propagation alone.


## Getting Started

### Prerequisites

- A modern C++ compiler that supports C++11 (e.g., GCC, Clang, MSVC)
- CMake for building the project (optional)

### Installation
To install the Sudoku Solver, follow these steps:

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/sudokusolver.git
   ```
2. Navigate to the project directory:
   ```bash
   cd sudokusolver
   ```
3. Compile the project (make sure you have a C++ compiler installed):
   ```bash
   clang++ -o SudokuSolver main.cpp SudokuSolver.cpp -std=c++11
   ```

## Usage
Run the solver using the following command:
```bash
./SudokuSolver
```

## Features

### Advanced Solving Techniques
- **AC-3 Algorithm**: Implements the AC-3 (Arc Consistency Algorithm #3) to reduce the search space by making the puzzle arc-consistent, which helps in solving easier puzzles without guessing.
- **Backtracking Algorithm**: Uses a sophisticated backtracking method that combines with the AC-3 reductions to solve more complex puzzles that cannot be resolved through direct constraint satisfaction alone.
- **Minimum Remaining Values Heuristic**: Leverages the well-established MRV heuristic, a powerful strategy widely used in constraint satisfaction problems. This heuristic efficiently prioritizes cells with the fewest possible numbers left, significantly accelerating the solving process.

### User-Friendly Interaction
- **Interactive Console Application**: Offers a simple and interactive console interface where users can input the path to a text file containing the Sudoku puzzle they wish to solve.
- **Dynamic File Handling**: Supports loading puzzles from `.txt` files, allowing users to easily test different puzzles without modifying the code.

### Optimized Performance
- **Efficient Algorithm Implementation**: Both the AC-3 and backtracking algorithms are optimized for performance, ensuring quick puzzle solving even for the most challenging puzzles.
- **Minimal Memory Usage**: Carefully manages memory, ensuring that the solver runs efficiently on all compatible devices.

### Customization and Extensibility
- **Modular Design**: The codebase is designed in a modular fashion, allowing for easy customization and extension. Developers can easily add new solving techniques or improve existing algorithms.
- **Plug-and-Play Functionality**: Designed to be flexible, allowing users to easily replace the puzzle input or integrate the solver into larger systems or other software.

## Contributing
Contributions to the Sudoku Solver are welcome! Please fork the repository and submit a pull request with your enhancements.

## License
This project is licensed under the MIT License - see the [MIT License Documentation](https://opensource.org/licenses/MIT) for details.

## Contact
For support, please feel free to contact me.
