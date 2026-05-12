CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

TARGET = library_system
SOURCES = src/main.cpp src/Book.cpp src/LibraryManagementSystem.cpp
HEADERS = src/Book.h src/RedBlackTree.h src/LibraryManagementSystem.h
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
