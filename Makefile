# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall

# Folders
SRC_DIR := src
BUILD_DIR := build
BIN := main

# Source, object, and header files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
HEADERS := $(wildcard src/*.h)

# Default target
all: run

# Build executable
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files: now depend on all headers
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run the program
run: $(BIN)
	./$(BIN)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN)

