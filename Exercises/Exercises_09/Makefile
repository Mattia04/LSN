# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -O3 -Wextra

# Directories
SRC_DIR = ./source
INC_DIR = ./include

# Source and object files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = main.exe


.PHONY: all clean


# Default target: build the executable
all: $(TARGET)

# Link object files into executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(INC_DIR)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)
