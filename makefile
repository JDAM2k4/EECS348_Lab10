# Makefile for String-Based Arithmetic Calculator
# -----------------------------------------------
# To compile:    make
# To run:        ./calculator
# To clean:      make clean

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Target executable name
TARGET = calculator

# Source file
SRC = main.cpp

# Object file
OBJ = $(SRC:.cpp=.o)

# Default rule
all: $(TARGET)

# Link step
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

# Compile step
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)
