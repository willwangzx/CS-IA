# Library Management System

A C++17 library management system built on top of a Red-Black Tree. The project now supports both a console application and an optional SFML GUI, persistent storage through `library.dat` plus `library.dat.journal`, and multiple physical copies of the same ISBN.

## Current Highlights

- **Red-Black Tree core** for `O(log n)` insert, remove, and search operations.
- **Persistent storage** via `library.dat` snapshots plus `library.dat.journal` write-ahead entries.
- **Duplicate copy support** so the same ISBN can exist multiple times, each with its own internal copy ID.
- **Animated Red-Black Tree visualization** in the SFML GUI for traced add/remove operations on the real catalog tree.
- **Two front ends**:
  - `library_system` for console usage.
  - `library_system_gui` for desktop usage when SFML is installed.
- **Cross-platform CMake build** with optional GUI compilation, install rules, and CPack packaging.

## Project Structure

| File | Purpose |
|------|---------|
| `Book.h` / `Book.cpp` | Book model with ISBN, title, author, year, availability, and copy ID |
| `RedBlackTree.h` | Generic Red-Black Tree implementation |
| `LibraryManagementSystem.h` / `.cpp` | Core library operations, persistence, and traversal helpers |
| `main.cpp` | Console entry point |
| `LibraryGUI.h` / `LibraryGUI.cpp` | GUI screens and reusable SFML UI widgets |
| `RBTreeVisualizer.h` / `RBTreeVisualizer.cpp` | SFML Red-Black Tree drawing, animation timeline, and invariant panel |
| `main_gui.cpp` | GUI bootstrap and font discovery |
| `CMakeLists.txt` | Recommended build, install, and packaging configuration |
| `Makefile` / `Makefile_complete` | Legacy build workflows |
| `setup.sh` | Linux helper script for SFML-related setup |

## What Changed From Earlier Versions

The current codebase behaves differently from the original classroom version:

- The backend loads `library.dat` at startup and replays `library.dat.journal` when pending journal entries exist.
- The console app starts from the same persisted catalog as the GUI and no longer inserts sample books automatically.
- Adding a second book with the same ISBN is allowed; it becomes a **new copy**.
- Removing, checkout, and return operations work on the **first matching copy** for the ISBN, with availability-aware logic for checkout and return.
- The GUI target is **optional** in CMake and only builds when SFML is detected.
- The GUI can locate fonts using `LIBRARY_GUI_FONT`, the CMake cache variable `LIBRARY_GUI_DEFAULT_FONT`, or common system font paths.
- The GUI includes a `Red-Black Tree View` screen that animates real add/remove tree operations with node colors, rotations, recoloring, and invariant checks.

## Build Instructions

### Recommended: CMake

Build the console app and, when SFML is available, the GUI app:

```bash
cmake -S . -B build
cmake --build build
```

Build only the console application:

```bash
cmake -S . -B build -DBUILD_GUI_APP=OFF
cmake --build build
```

Configure an explicit default GUI font path:

```bash
cmake -S . -B build -DLIBRARY_GUI_DEFAULT_FONT=/absolute/path/to/font.ttf
```

Install into a staging directory:

```bash
cmake --install build --prefix ./dist
```

Create a distributable archive:

```bash
cd build
cpack
```

### Legacy Makefiles

Console-only build:

```bash
make
```

Console + GUI build:

```bash
make -f Makefile_complete all
```

## Running

### Console

```bash
./build/library_system
```

Or, if using the makefile:

```bash
./library_system
```

### GUI

```bash
./build/library_system_gui
```

If the GUI cannot find a font automatically, set one explicitly:

```bash
LIBRARY_GUI_FONT=/absolute/path/to/font.ttf ./build/library_system_gui
```

## Runtime Behavior

### Persistence

The backend reads from `library.dat` during `LibraryManagementSystem` construction and then replays `library.dat.journal` if it exists. Successful add, remove, checkout, and return operations append write-ahead journal entries. The catalog is compacted back into `library.dat` when the journal reaches the compaction threshold or when the backend is destroyed.

Both front ends use the same persisted files. If the catalog files are missing, the application starts with an empty library.

### Multiple Copies

Every book record has:

- **ISBN**: groups editions/copies logically.
- **Copy ID**: assigned automatically per ISBN and used to distinguish duplicate copies.
- **Availability**: tracked per copy.

Example:

- First call to `addBook(1001, ...)` creates ISBN `1001`, copy `1`.
- Second call to `addBook(1001, ...)` creates ISBN `1001`, copy `2`.

### Search and Catalog Rules

- `findBook` / console search returns the **first matching copy** for a given ISBN.
- Checkout picks the **first available copy** for the ISBN.
- Return picks the **first checked-out copy** for the ISBN.
- Remove deletes the **first matching copy** for the ISBN.
- Full catalog display is sorted by **ISBN, then copy ID**.

### Red-Black Tree Visualization

The SFML GUI main menu includes `Red-Black Tree View`. This screen draws the actual `RedBlackTree<Book>` used by the backend, not a separate sample tree. `Add & Animate` and `Remove & Animate` call traced backend operations, append the normal journal entries, mutate the real tree, and play the recorded algorithm steps. The visualizer shows node colors, active comparisons, transplants, rotations, recoloring, a bottom timeline, selected-book details, and root-black / no-red-red / black-height checks.

## Complexity

| Operation | Complexity |
|-----------|------------|
| Insert | `O(log n)` tree insertion, plus journal append |
| Remove first matching ISBN | `O(log n)` |
| Find first matching ISBN | `O(log n)` |
| Checkout/return first relevant copy | `O(log n + k)`, where `k` is the number of matching ISBN copies scanned |
| Display all books | `O(n)` |
| Build tree visualization snapshot | `O(n)` |
| Compact/save catalog | `O(n)` |

## Development Notes

- Mutating operations normally append to `library.dat.journal`; `compactSave()` rewrites the full `library.dat` snapshot.
- The serialized file format stores ISBN, copy ID, title, author, year, and availability. Older five-field rows are still accepted for backward compatibility.
- `Makefile_complete` assumes SFML linker flags are already available on the system; CMake is more portable and is the preferred workflow.

## License

Free to use for educational purposes.
