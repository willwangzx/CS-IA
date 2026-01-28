# Quick Start Guide - Library Management System GUI

## Prerequisites Check

Before you begin, ensure you have:
- [ ] Linux system (Ubuntu 24 or similar)
- [ ] C++17 compatible compiler (g++ 7.0+)
- [ ] SFML 3.0.2 libraries
- [ ] X Window System for GUI display

## Installation Steps

### 1. Automatic Setup (Recommended)

Run the setup script to install all dependencies:

```bash
chmod +x setup.sh
./setup.sh
```

The script will:
- Install build tools
- Install required libraries
- Install fonts
- Optionally build SFML 3.0.2 from source

### 2. Manual Setup (Alternative)

If you prefer manual installation:

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake git libsfml-dev fonts-dejavu

# Or build SFML 3.0.2 from source (see README_GUI.md)
```

## Building the Project

### Option A: Build Everything
```bash
make -f Makefile_complete all
```

This creates:
- `library_system` (console version)
- `library_system_gui` (GUI version)

### Option B: Build Only What You Need

For GUI only:
```bash
make -f Makefile_complete gui
```

For console only:
```bash
make -f Makefile_complete console
```

## Running the Application

### Launch GUI Version
```bash
./library_system_gui
```

### Launch Console Version
```bash
./library_system
```

## First Time Usage

### GUI Version

1. **Main Menu** appears automatically
2. Click on any option to begin:
   - Start with "View All Books" to see sample data
   - Try "Add Book" to create a new entry
   - Use "Search Book" to find specific books

3. **Sample Data**
   The system comes pre-loaded with 5 classic books:
   - The Great Gatsby (ISBN: 1001)
   - To Kill a Mockingbird (ISBN: 1002)
   - 1984 (ISBN: 1003)
   - Pride and Prejudice (ISBN: 1004)
   - The Catcher in the Rye (ISBN: 1005)

### Adding Your First Book

1. Click "Add Book" on main menu
2. Fill in the form:
   - **ISBN**: 2001 (must be unique)
   - **Title**: Clean Code
   - **Author**: Robert C. Martin
   - **Year**: 2008
3. Click "Add Book" button
4. Success message appears at bottom
5. Click "Back" to return to main menu

### Searching for a Book

1. Click "Search Book"
2. Enter ISBN: 1001
3. Click "Search"
4. Book details appear in the list below

### Checking Out a Book

1. Click "Checkout Book"
2. Enter ISBN: 1001
3. Click "Checkout"
4. Success message confirms checkout

### Returning a Book

1. Click "Return Book"
2. Enter the same ISBN: 1001
3. Click "Return"
4. Success message confirms return

## Common Operations

### View All Books
- Click "View All Books" from main menu
- Scroll using mouse wheel
- Click "Back" to return

### Remove a Book
- Click "Remove Book"
- Enter ISBN of book to delete
- Click "Remove"
- Confirmation message appears

## Keyboard and Mouse Guide

### Mouse
- **Left Click**: Activate buttons, focus input fields
- **Mouse Wheel**: Scroll through book lists
- **Hover**: Buttons change color when mouse is over them

### Keyboard
- **Typing**: Enter text in focused fields (blue border)
- **Backspace**: Delete characters
- **Numbers/Letters**: Input data

## Troubleshooting

### "Error: Could not load font!"

**Solution 1**: Update font path in `main_gui.cpp`
```bash
# Find your font location
find /usr/share/fonts -name "*.ttf"

# Common locations:
# /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf
# /usr/share/fonts/TTF/DejaVuSans.ttf
```

**Solution 2**: Install DejaVu fonts
```bash
sudo apt-get install fonts-dejavu
```

### "SFML library not found"

**Solution**: Add to library path
```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
./library_system_gui
```

Or make permanent:
```bash
echo 'export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```

### Compilation Fails

**Check C++ version**:
```bash
g++ --version  # Should be 7.0 or higher
```

**Check SFML**:
```bash
pkg-config --list-all | grep sfml
```

### Window Doesn't Appear

**Check X server**:
```bash
echo $DISPLAY  # Should output something like :0
```

**For WSL users**:
Install an X server like VcXsrv or Xming on Windows.

## Tips and Tricks

1. **Navigation**: Use the "Back" button on any screen to return to main menu
2. **Input Focus**: Click inside any input field to activate it (blue border = active)
3. **Messages**: Watch the bottom of the screen for success/error messages
4. **Scrolling**: In "View All Books", use mouse wheel to see more entries
5. **ISBN Format**: Use unique integers for each book

## System Limits

- **ISBN**: Must be unique integer
- **Title**: Any text
- **Author**: Any text  
- **Year**: Integer (typically 1000-2100)
- **Performance**: Handles thousands of books efficiently (O(log n) operations)

## Data Persistence

⚠️ **Important**: Data is NOT saved between sessions. When you close the application, all data (except the 5 sample books) is lost.

For persistent storage, see the Future Enhancements section in README_GUI.md.

## Next Steps

1. Explore all menu options
2. Add your own books
3. Test search functionality
4. Try checkout/return operations
5. Review the technical documentation in README.md

## Getting Help

- Read the full documentation: `README_GUI.md`
- Review architecture details: `README.md`
- Check source code comments
- Verify SFML installation: `./setup.sh`

## Clean Up

To remove compiled files:
```bash
make -f Makefile_complete clean
```

To completely uninstall (removes executables):
```bash
make -f Makefile_complete clean
rm -f library_system library_system_gui
```

---

**Ready to start?** Run `./library_system_gui` and enjoy! 📚✨
