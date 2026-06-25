# Library Management System - GUI Guide

This document describes the **current** SFML GUI behavior for the project.

## Overview

The GUI is an optional desktop front end for the same `LibraryManagementSystem` backend used by the console application. It provides screens for adding, removing, searching, checking out, returning, listing books, and visualizing the Red-Black Tree.

## Current GUI Characteristics

- Built with **C++17 + SFML**.
- Uses the same persistent backend as the console app.
- Loads existing records from `library.dat` and replays `library.dat.journal` through `LibraryManagementSystem`.
- Displays books in a scrollable list.
- Animates real Red-Black Tree add/remove operations with node colors, rotations, recoloring, and invariant checks.
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
- Red-Black Tree View

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
- Shows either a success message or `Book not found!`

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
- If no copy is available, the GUI reports either `Book not found!` or `All copies are already checked out.`

### Return Book
Inputs:
- ISBN

Behavior:
- Returns the **first checked-out copy** for that ISBN
- If no checked-out copy exists, the GUI reports either `Book not found!` or `All copies are already available.`

### View All Books
Behavior:
- Rebuilds the list from `library.forEachBook(...)`
- Shows the current catalog order from the Red-Black Tree
- Supports mouse-wheel scrolling
- Displays status as `Available` or `Checked Out`

### Red-Black Tree View
Inputs:
- ISBN
- Title
- Author
- Year

Behavior:
- Draws the real backend `RedBlackTree<Book>` with red nodes, black nodes, edges, selected nodes, and small nil leaves for small trees.
- `Add & Animate` validates the fields, writes the normal journal entry, inserts the new copy into the real tree, and plays the traced insertion steps.
- `Remove & Animate` removes the first matching copy for the ISBN and plays the traced deletion, successor, transplant, rotation, recolor, and completion steps.
- `Play/Pause`, `Step`, `Reset`, `Fit`, and `Refresh` control the timeline display; reset does not roll back committed catalog changes.
- The side panel reports the current algorithm step, legend, selected book, node count, height, black height, and the root-black / no-red-red / uniform-black-height invariants.

## Data and Persistence

The GUI itself does not inject sample books. Instead:
- the backend loads `library.dat` at startup and replays `library.dat.journal` when present
- successful mutations append journal entries
- the journal is compacted back into `library.dat` at the compaction threshold or backend shutdown
- changes made in the GUI are visible to the console app and vice versa across runs through the shared file
- neither front end inserts sample books automatically

## Known Limitations

- Search returns only the first matching copy, not every copy with the same ISBN.
- GUI operations choose the first relevant copy automatically rather than allowing the user to select an exact copy ID.
- Text fields are plain free-form inputs with minimal validation.
- Red-Black Tree animation add/remove operations mutate the real persisted catalog; the screen is not a disposable sandbox.

## Development Notes

Prefer the CMake workflow over `Makefile_complete` for portability. CMake also handles the optional GUI target more gracefully by skipping it when SFML is unavailable.
