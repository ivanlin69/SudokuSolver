TARGET = bin/SudokuSolver

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))

run: clean default
	./$(TARGET)

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*

$(TARGET): $(OBJ)
	clang++ --std=c++11 -o $@ $^

obj/%.o: src/%.cpp
	clang++ --std=c++11 -c $< -o $@ -Iinclude
