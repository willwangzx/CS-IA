# Library Management System - GUI Version

A modern graphical user interface for the Library Management System, built with SFML 3.0.2 and C++17.

## Features

### Visual Interface
- **Modern Design**: Clean, professional interface with intuitive navigation
- **Interactive Buttons**: Hover effects and visual feedback
- **Input Forms**: Easy-to-use text input fields with focus indicators
- **Message Notifications**: Temporary pop-up messages for user feedback
- **Scrollable Lists**: View large collections of books with mouse wheel scrolling

### Functionality
- Add new books with complete information
- Remove books from the library
- Search for books by ISBN
- Check out books
- Return books
- View all books in a scrollable list

## Requirements

### Dependencies
- **SFML 3.0.2** (Simple and Fast Multimedia Library)
  - sfml-graphics
  - sfml-window
  - sfml-system
- **C++17 compiler** (g++ 7.0 or later)

### System Requirements
- Linux (Ubuntu 24 recommended)
- X Window System for display
- DejaVu Sans font (usually pre-installed)

## Installation

### Install SFML 3.0.2

#### Option 1: From Source (Recommended for SFML 3.0.2)
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install libx11-dev libxrandr-dev libxcursor-dev libxi-dev \
                     libudev-dev libgl1-mesa-dev libfreetype-dev \
                     libopenal-dev libvorbis-dev libflac-dev cmake

# Download and build SFML 3.0.2
git clone https://github.com/SFML/SFML.git
cd SFML
git checkout 3.0.2
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

#### Option 2: Package Manager (may not have 3.0.2)
```bash
sudo apt-get install libsfml-dev
```

### Install Font
```bash
sudo apt-get install fonts-dejavu
```

## Compilation

### Using the Makefile

Build both console and GUI versions:
```bash
make -f Makefile_complete all
```

Build only GUI version:
```bash
make -f Makefile_complete gui
```

Build only console version:
```bash
make -f Makefile_complete console
```

### Manual Compilation

Console version:
```bash
g++ -std=c++17 -Wall -Wextra -o library_system main.cpp Book.cpp LibraryManagementSystem.cpp
```

GUI version:
```bash
g++ -std=c++17 -Wall -Wextra -o library_system_gui main_gui.cpp LibraryGUI.cpp \
    Book.cpp LibraryManagementSystem.cpp \
    -lsfml-graphics -lsfml-window -lsfml-system
```

## Running the Application

### GUI Version
```bash
./library_system_gui
```

Or using Make:
```bash
make -f Makefile_complete run-gui
```

### Console Version
```bash
./library_system
```

Or using Make:
```bash
make -f Makefile_complete run-console
```

## User Interface Guide

### Main Menu
The main menu presents six primary options:
1. **Add Book** - Add a new book to the library
2. **Remove Book** - Remove an existing book
3. **Search Book** - Find a book by ISBN
4. **Checkout Book** - Mark a book as checked out
5. **Return Book** - Mark a book as returned
6. **View All Books** - See the complete catalog

### Add Book Screen
- **ISBN**: Enter a unique numeric identifier
- **Title**: Enter the book's title
- **Author**: Enter the author's name
- **Year**: Enter the publication year

Click "Add Book" to save, or "Back" to return to the main menu.

### Remove Book Screen
- Enter the ISBN of the book you want to remove
- Click "Remove" to delete the book

### Search Book Screen
- Enter an ISBN to search
- Click "Search" to display book details
- Results appear in the list below

### Checkout/Return Screens
- Enter the ISBN of the book
- Click the appropriate button to check out or return

### View All Books Screen
- Displays all books in the library
- Use mouse wheel to scroll through the list
- Books shown in order by ISBN

## UI Components

### Buttons
- **Hover Effect**: Buttons change color when mouse hovers over them
- **Click**: Left-click to activate a button
- **Visual Feedback**: Active buttons show a different color

### Input Fields
- **Focus**: Click on a field to activate it (blue border indicates focus)
- **Typing**: Type directly into focused fields
- **Backspace**: Delete characters
- **Tab**: Move to next field (if implemented)

### Message Box
- Appears at the bottom center of the window
- Shows success/error messages
- Automatically disappears after 3 seconds

### Scrollable Lists
- Use mouse wheel to scroll up/down
- Displays up to 10 items at a time
- Shows book details in a readable format

## Color Scheme

- **Primary Color**: Steel Blue (70, 130, 180)
- **Hover Color**: Cornflower Blue (100, 149, 237)
- **Active Color**: Royal Blue (65, 105, 225)
- **Background**: Light Gray (245, 245, 245)
- **Text**: Black for readability

## Architecture

### Class Structure

#### LibraryGUI
Main GUI controller that manages screens and user interactions.

#### Button
Reusable button component with hover effects.

#### InputBox
Text input field with focus management and cursor display.

#### ScrollableList
Scrollable list for displaying multiple items.

#### MessageBox
Temporary notification system for user feedback.

## Keyboard Shortcuts

- **Backspace**: Delete characters in input fields
- **Mouse Wheel**: Scroll in list views
- **ESC**: (Can be implemented) Return to main menu

## Troubleshooting

### Font Loading Error
If you see "Error: Could not load font!", try:
```bash
# Locate DejaVu font
find /usr/share/fonts -name "DejaVuSans.ttf"

# Update the path in main_gui.cpp
```

### SFML Library Not Found
```bash
# Add library path
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Or install to system directory
sudo ldconfig
```

### Compilation Errors
Ensure you have C++17 support:
```bash
g++ --version  # Should be 7.0 or higher
```

Check SFML version:
```bash
pkg-config --modversion sfml-all
```

## Known Limitations

- View All screen shows initial sample books (static list)
- No persistent file storage (data lost on exit)
- Single-user system (no concurrent access)
- Basic input validation

## Future Enhancements

Potential GUI improvements:
- Real-time book list updates
- Multi-column sorting
- Search by title/author
- Book cover images
- Date picker for due dates
- User account management
- Statistics dashboard
- Print functionality
- Export to PDF/CSV
- Dark mode theme
- Keyboard navigation
- Drag-and-drop file import
- Barcode scanning integration

## Performance

- **Frame Rate**: Locked at 60 FPS for smooth animations
- **Memory**: Minimal overhead for typical library sizes
- **Responsiveness**: Immediate visual feedback on all interactions

## File Structure

```
library-management-system/
├── Book.h / Book.cpp              # Book entity
├── RedBlackTree.h                 # Red-Black Tree implementation
├── LibraryManagementSystem.h/cpp  # Library logic
├── LibraryGUI.h / LibraryGUI.cpp  # GUI implementation
├── main.cpp                       # Console entry point
├── main_gui.cpp                   # GUI entry point
├── Makefile_complete              # Build system
└── README_GUI.md                  # This file
```

## Credits

Built with:
- SFML 3.0.2 by Laurent Gomila
- DejaVu Fonts Project
- C++ Standard Library

## License

Free to use for educational purposes.

## Support

For issues or questions:
1. Check the troubleshooting section
2. Verify SFML installation
3. Ensure all dependencies are met
4. Check font paths

Enjoy your modern library management experience! 📚
