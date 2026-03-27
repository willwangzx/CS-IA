# Library Management System - GUI Guide

This document describes the **current** SFML GUI behavior for the project.

## Overview

The GUI is an optional desktop front end for the same `LibraryManagementSystem` backend used by the console application. It provides screens for adding, removing, searching, checking out, returning, and listing books.

## Current GUI Characteristics

- Built with **C++17 + SFML**.
- Uses the same persistent backend as the console app.
- Loads existing records from `library.dat` through `LibraryManagementSystem`.
- Displays books in a scrollable list.
- Uses simple success/error message boxes for feedback.
- Relies on runtime font discovery instead of one hard-coded font path.

## Build

### Recommended
```bash
cmake -S . -B build
cmake --build build
```

If SFML is detected, this produces `build/library_system_gui`.

### With an explicit default font
```bash
cmake -S . -B build -DLIBRARY_GUI_DEFAULT_FONT=/absolute/path/to/font.ttf
cmake --build build
```

### Legacy makefile build
```bash
make -f Makefile_complete gui
```

## Running

```bash
./build/library_system_gui
```

If automatic font detection fails:

```bash
LIBRARY_GUI_FONT=/absolute/path/to/font.ttf ./build/library_system_gui
```

## Font Resolution Strategy

The GUI tries fonts in this order:

1. `LIBRARY_GUI_FONT`
2. `LIBRARY_GUI_DEFAULT_FONT` set during CMake configure
3. OS-specific common font locations

If none of these work, the application exits with a font-loading error.

## Screen-by-Screen Behavior

### Main Menu
Provides navigation to:
- Add Book
- Remove Book
- Search Book
- Checkout Book
- Return Book
- View All Books

### Add Book
Inputs:
- ISBN
- Title
- Author
- Year

Behavior:
- Converts ISBN and year with `std::stoi`
- Calls backend `addBook`
- Adding the same ISBN again creates a new **copy**
- Shows a success message and clears inputs on valid parsing

### Remove Book
Inputs:
- ISBN

Behavior:
- Removes the **first matching copy** for that ISBN
- Shows a generic success message when parsing succeeds
- Backend failure details are printed to stdout rather than fully reflected in GUI state

### Search Book
Inputs:
- ISBN

Behavior:
- Displays the **first matching copy** for that ISBN
- Shows one result row or `Book not found.`

### Checkout Book
Inputs:
- ISBN

Behavior:
- Checks out the **first available copy** for that ISBN
- If all copies are already checked out, the backend prints that detail to stdout

### Return Book
Inputs:
- ISBN

Behavior:
- Returns the **first checked-out copy** for that ISBN
- If all copies are already available, the backend prints that detail to stdout

### View All Books
Behavior:
- Rebuilds the list from `library.forEachBook(...)`
- Shows the current catalog order from the Red-Black Tree
- Supports mouse-wheel scrolling
- Displays status as `Available` or `Checked Out`

## Data and Persistence

The GUI itself does not inject sample books. Instead:
- the backend loads `library.dat` at startup
- successful mutations save back to `library.dat`
- changes made in the GUI are visible to the console app and vice versa across runs through the shared file
- if the console app has been launched before, its startup seeding will already have added the five sample books to persisted data

## Known Limitations

- GUI messages do not always distinguish backend success from backend refusal after a valid ISBN parse.
- Search returns only the first matching copy, not every copy with the same ISBN.
- The serialized data format does not explicitly persist copy IDs.
- Text fields are plain free-form inputs with minimal validation.

## Development Notes

Prefer the CMake workflow over `Makefile_complete` for portability. CMake also handles the optional GUI target more gracefully by skipping it when SFML is unavailable.
