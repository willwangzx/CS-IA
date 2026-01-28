# Library Management System - Complete Project Overview

A professional library management system featuring both command-line and graphical interfaces, built with C++17 and SFML 3.0.2, utilizing a Red-Black Tree for efficient data management.

## 📦 Project Contents

### Core Files
| File | Description |
|------|-------------|
| `Book.h` / `Book.cpp` | Book entity with ISBN, title, author, year, and availability |
| `RedBlackTree.h` | Self-balancing binary search tree template implementation |
| `LibraryManagementSystem.h` / `.cpp` | Library business logic and operations |

### Console Interface
| File | Description |
|------|-------------|
| `main.cpp` | Console-based interactive menu program |

### GUI Interface (SFML 3.0.2)
| File | Description |
|------|-------------|
| `LibraryGUI.h` / `LibraryGUI.cpp` | Complete GUI implementation with screens and components |
| `main_gui.cpp` | GUI application entry point |

### Build System
| File | Description |
|------|-------------|
| `Makefile` | Original console-only makefile |
| `Makefile_complete` | Full makefile for both console and GUI versions |
| `setup.sh` | Automated installation script for dependencies |

### Documentation
| File | Description |
|------|-------------|
| `README.md` | Original console version documentation |
| `README_GUI.md` | Comprehensive GUI documentation |
| `QUICKSTART.md` | Quick start guide for new users |
| `GUI_ARCHITECTURE.md` | Detailed architecture and design documentation |

## 🎯 Features

### Data Structure
- **Red-Black Tree** implementation
  - Self-balancing binary search tree
  - O(log n) insert, delete, and search operations
  - Maintains sorted order automatically
  - Efficient for large datasets

### Console Features
- Interactive menu-driven interface
- Add/remove books
- Search by ISBN
- Checkout/return tracking
- Display all books in sorted order
- Input validation

### GUI Features
- Modern, professional interface
- Interactive buttons with hover effects
- Text input fields with focus indicators
- Scrollable book lists
- Real-time message notifications
- Mouse and keyboard support
- 60 FPS smooth rendering

### Operations
- **Add Book**: Insert new books with complete information
- **Remove Book**: Delete books by ISBN
- **Search Book**: Find books quickly (O(log n))
- **Checkout Book**: Mark books as checked out
- **Return Book**: Mark books as available
- **View All**: Display complete catalog in sorted order

## 🏗️ Architecture

### Class Hierarchy
```
Book (Data Model)
  ↓
RedBlackTree<Book> (Data Structure)
  ↓
LibraryManagementSystem (Business Logic)
  ↓
Main Programs:
  - main.cpp (Console Interface)
  - LibraryGUI + main_gui.cpp (Graphical Interface)
```

### Red-Black Tree Properties
1. Every node is either red or black
2. Root is always black
3. All leaves (NIL) are black
4. Red nodes cannot have red children
5. All paths from root to leaves contain same number of black nodes

### GUI Components
- **Button**: Clickable UI element with states (normal, hover, active)
- **InputBox**: Text input field with focus management
- **ScrollableList**: Scrollable container for displaying multiple items
- **MessageBox**: Temporary notification system
- **Screen Manager**: Handles navigation between different views

## 📊 Performance Characteristics

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Insert    | O(log n)       | O(1)             |
| Delete    | O(log n)       | O(1)             |
| Search    | O(log n)       | O(1)             |
| Display All | O(n)         | O(log n)         |

Where n = number of books in the library

## 🚀 Quick Start

### Prerequisites
- Linux OS (Ubuntu 24 recommended)
- C++17 compiler (g++ 7.0+)
- SFML 3.0.2 (for GUI version)
- DejaVu Sans font (usually pre-installed)

### Installation
```bash
# Automatic setup
./setup.sh

# Or manual
sudo apt-get install build-essential libsfml-dev fonts-dejavu
```

### Building
```bash
# Both versions
make -f Makefile_complete all

# GUI only
make -f Makefile_complete gui

# Console only
make -f Makefile_complete console
```

### Running
```bash
# GUI version
./library_system_gui

# Console version
./library_system
```

## 📚 Sample Data

The system comes pre-loaded with 5 classic books:

| ISBN | Title | Author | Year |
|------|-------|--------|------|
| 1001 | The Great Gatsby | F. Scott Fitzgerald | 1925 |
| 1002 | To Kill a Mockingbird | Harper Lee | 1960 |
| 1003 | 1984 | George Orwell | 1949 |
| 1004 | Pride and Prejudice | Jane Austen | 1813 |
| 1005 | The Catcher in the Rye | J.D. Salinger | 1951 |

## 🎨 Design Specifications

### GUI Design
- **Window Size**: 900 x 700 pixels
- **Frame Rate**: 60 FPS
- **Color Scheme**: Professional blue-gray theme
  - Primary: Steel Blue (70, 130, 180)
  - Hover: Cornflower Blue (100, 149, 237)
  - Active: Royal Blue (65, 105, 225)
  - Background: Light Gray (245, 245, 245)

### Fonts
- Header: 32pt Bold
- Screen Titles: 24pt Bold
- Buttons: 18pt Regular
- Input/Labels: 16-18pt Regular
- Lists: 14pt Regular

## 💡 Usage Examples

### Adding a Book (GUI)
1. Click "Add Book" from main menu
2. Fill in the form:
   - ISBN: 2001
   - Title: Clean Code
   - Author: Robert C. Martin
   - Year: 2008
3. Click "Add Book" button
4. Success message appears

### Searching (Console)
```
Enter your choice: 3
Enter ISBN to search: 1001
ISBN: 1001 | Title: The Great Gatsby | Author: F. Scott Fitzgerald | Year: 1925 | Status: Available
```

### Checkout/Return
- Checkout: Marks book as unavailable
- Return: Marks book as available again
- System tracks status automatically

## 🔧 Technical Details

### Red-Black Tree Implementation
- Template-based design (works with any comparable type)
- Sentinel NIL nodes for simplified logic
- Left and right rotation operations
- Color-flipping for rebalancing
- Iterative and recursive operations

### Memory Management
- Smart pointers (unique_ptr) for GUI components
- RAII principles throughout
- Proper cleanup in destructors
- No memory leaks

### Thread Safety
- Single-threaded design
- No concurrent access issues
- Suitable for single-user desktop application

## 📈 Scalability

The Red-Black Tree ensures:
- Efficient operations even with thousands of books
- Guaranteed O(log n) worst-case performance
- Balanced tree structure maintained automatically
- No degradation to O(n) unlike simple BST

For 10,000 books:
- Search: ~14 comparisons maximum
- Insert: ~14 comparisons + rebalancing
- Delete: ~14 comparisons + rebalancing

## 🔍 Testing Recommendations

### Functional Tests
- [ ] Add books with various ISBNs
- [ ] Remove existing and non-existing books
- [ ] Search for present and absent books
- [ ] Checkout available books
- [ ] Return checked-out books
- [ ] View all books
- [ ] Input validation (invalid ISBN, empty fields)

### Edge Cases
- [ ] Duplicate ISBN handling
- [ ] Empty library operations
- [ ] Very large ISBN numbers
- [ ] Special characters in titles/authors
- [ ] Extremely long strings

### GUI-Specific Tests
- [ ] Button hover effects
- [ ] Input field focus management
- [ ] Scroll functionality
- [ ] Message box display/timeout
- [ ] Screen transitions
- [ ] Window resize/close

## 🚧 Known Limitations

1. **No Persistent Storage**: Data lost when application closes
2. **Single User**: No multi-user support
3. **Basic Search**: Only by ISBN (not by title/author)
4. **No Date Tracking**: No due dates for checkouts
5. **Static View All**: List doesn't auto-update after operations

## 🔮 Future Enhancements

### High Priority
- [ ] File-based persistence (save/load library state)
- [ ] Search by title and author
- [ ] Due date tracking for checkouts
- [ ] Overdue book notifications
- [ ] Book cover image support

### Medium Priority
- [ ] User account system
- [ ] Fine calculation
- [ ] Reservation system
- [ ] Category/genre classification
- [ ] Statistics dashboard

### Low Priority
- [ ] Database integration (SQLite)
- [ ] Network support (client-server)
- [ ] Barcode scanning
- [ ] Export to PDF/CSV
- [ ] Print functionality
- [ ] Dark mode theme

## 🐛 Troubleshooting

### Common Issues

**Font Not Found**
```bash
find /usr/share/fonts -name "DejaVuSans.ttf"
# Update path in main_gui.cpp
```

**SFML Not Found**
```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
sudo ldconfig
```

**Compilation Errors**
```bash
g++ --version  # Ensure 7.0+
pkg-config --list-all | grep sfml
```

## 📝 Development Notes

### Code Style
- C++17 standard
- RAII principles
- Smart pointers
- Const correctness
- Clear naming conventions

### Design Patterns
- Template Method (Red-Black Tree)
- State Pattern (GUI screens)
- Observer (GUI events)
- Factory (Component creation)

## 📄 License

Free to use for educational purposes.

## 🤝 Contributing

Potential contributions:
- Add database persistence
- Implement search by title/author
- Add book cover images
- Create unit tests
- Add due date system
- Improve UI/UX

## 📞 Support

For help:
1. Check QUICKSTART.md
2. Review README_GUI.md
3. Read GUI_ARCHITECTURE.md
4. Run setup.sh for dependency check
5. Verify font paths

## 🎓 Educational Value

This project demonstrates:
- Advanced data structures (Red-Black Tree)
- Object-oriented programming
- GUI development with SFML
- Memory management
- Event-driven programming
- Template programming
- Build systems (Make)
- Software architecture

Perfect for:
- Data structures courses
- C++ programming classes
- Software engineering projects
- Portfolio pieces

## 📚 Resources

- SFML Documentation: https://www.sfml-dev.org/documentation/3.0.0/
- Red-Black Trees: Introduction to Algorithms (CLRS)
- C++17 Reference: https://en.cppreference.com/

---

**Version**: 1.0  
**Last Updated**: January 2026  
**Authors**: Educational Project  

Enjoy building and extending this library management system! 📚✨
