CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g

TARGET = library_system
SOURCES = main.cpp Book.cpp LibraryManagementSystem.cpp
HEADERS = Book.h RedBlackTree.h LibraryManagementSystem.h
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
