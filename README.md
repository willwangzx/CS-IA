# Library Management System with Red-Black Tree

A C++ implementation of a library management system using a Red-Black Tree data structure for efficient book storage and retrieval.

## Features

- **Efficient Operations**: O(log n) time complexity for insert, delete, and search operations
- **Book Management**: Add, remove, and search for books
- **Check-out System**: Track which books are available or checked out
- **Sorted Display**: View all books in sorted order by ISBN

## Data Structure

The system uses a **Red-Black Tree**, which is a self-balancing binary search tree with the following properties:

1. Every node is either red or black
2. The root is always black
3. All leaves (NIL nodes) are black
4. Red nodes cannot have red children
5. Every path from root to leaf contains the same number of black nodes

This ensures the tree remains balanced, guaranteeing O(log n) performance.

## Files

- `Book.h/cpp`: Book class representing library items
- `RedBlackTree.h`: Template-based Red-Black Tree implementation
- `LibraryManagementSystem.h/cpp`: Library system interface
- `main.cpp`: Interactive menu-driven program
- `Makefile`: Build automation

## Compilation

### Using Make:
```bash
make
```

### Manual Compilation:
```bash
g++ -std=c++11 -Wall -Wextra -o library_system main.cpp Book.cpp LibraryManagementSystem.cpp
```

## Running the Program

```bash
./library_system
```

Or using Make:
```bash
make run
```

## Usage

The program provides an interactive menu with the following options:

1. **Add a book**: Insert a new book into the library
2. **Remove a book**: Delete a book from the library
3. **Search for a book**: Find and display book details by ISBN
4. **Check out a book**: Mark a book as checked out
5. **Return a book**: Mark a book as available
6. **Display all books**: Show all books in sorted order
7. **Exit**: Quit the program

## Example Usage

```
===== Library Management System =====
1. Add a book
2. Remove a book
3. Search for a book
4. Check out a book
5. Return a book
6. Display all books
7. Exit
=====================================
Enter your choice: 1

Enter ISBN: 2001
Enter title: Clean Code
Enter author: Robert C. Martin
Enter year: 2008
Book added successfully: Clean Code
```

## Book Attributes

Each book contains:
- **ISBN** (International Standard Book Number): Unique identifier
- **Title**: Name of the book
- **Author**: Book's author
- **Year**: Publication year
- **Availability**: Whether the book is available or checked out

## Red-Black Tree Operations

### Insert
- Adds a new book maintaining Red-Black Tree properties
- Time Complexity: O(log n)

### Delete
- Removes a book while preserving tree balance
- Time Complexity: O(log n)

### Search
- Finds a book by ISBN
- Time Complexity: O(log n)

### Traversal
- In-order traversal displays books sorted by ISBN
- Time Complexity: O(n)

## Implementation Details

The Red-Black Tree uses:
- **Sentinel NIL nodes**: Simplifies boundary conditions
- **Rotations**: Left and right rotations for rebalancing
- **Color flipping**: Maintains Red-Black properties during insertion/deletion
- **Template-based design**: Can be used with any comparable data type

## Complexity Analysis

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Insert    | O(log n)       | O(1)             |
| Delete    | O(log n)       | O(1)             |
| Search    | O(log n)       | O(1)             |
| Display All | O(n)         | O(log n)*        |

*Due to recursion stack depth

## Future Enhancements

Potential improvements:
- Search by title or author
- Due date tracking for checked-out books
- Fine calculation for overdue books
- Persistent storage (file I/O)
- User account management
- Reservation system
- Category/genre classification

## Clean Up

To remove compiled files:
```bash
make clean
```

## Author

This project demonstrates the practical application of advanced data structures in real-world systems.

## License

Free to use for educational purposes.
