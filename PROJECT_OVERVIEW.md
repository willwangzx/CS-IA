# Library Management System - Project Overview

This project is a C++17 library manager with a Red-Black Tree backend, persistent file storage, duplicate-copy handling, a console UI, and an optional SFML desktop GUI.

## 1. System Overview

### Core Capabilities
- Store books in a balanced Red-Black Tree.
- Load and save the catalog to `library.dat`.
- Support multiple copies of the same ISBN.
- Track availability on a per-copy basis.
- Expose both console and GUI workflows over the same backend.

### Current Front Ends
- **Console app**: `main.cpp`
- **GUI app**: `main_gui.cpp` + `LibraryGUI.cpp`

## 2. Main Components

| Component | Description |
|-----------|-------------|
| `Book` | Data model containing ISBN, copy ID, title, author, year, and availability |
| `RedBlackTree<Book>` | Ordered storage layer keyed by ISBN and copy ID |
| `LibraryManagementSystem` | Business logic for add/remove/search/checkout/return, persistence, and iteration |
| `LibraryGUI` | SFML-based interface with menu screens, inputs, and message boxes |

## 3. Important Recent Changes

### Persistent Storage
The system now calls `loadFromFile("library.dat")` in the `LibraryManagementSystem` constructor and writes the catalog back after successful mutations.

### Duplicate ISBN Support
Books are no longer unique by ISBN alone. Comparison operators now use **ISBN first, then copy ID**, which allows multiple nodes with the same ISBN to coexist in the Red-Black Tree.

### Copy-Aware Operations
- **Add**: assigns the next copy ID for that ISBN.
- **Remove**: removes the first matching copy.
- **Checkout**: targets the first available copy.
- **Return**: targets the first checked-out copy.
- **Search**: returns the first matching copy.

### Modern Build Flow
CMake is now the preferred build system. It can:
- Build the shared core logic as a static library.
- Optionally build the GUI only when SFML is found.
- Install headers, binaries, and CMake export targets.
- Package releases with CPack.

## 4. File Map

### Backend
| File | Role |
|------|------|
| `Book.h` / `Book.cpp` | Book representation and serialization |
| `RedBlackTree.h` | Tree storage and traversal utilities |
| `LibraryManagementSystem.h` / `.cpp` | Backend API and persistence logic |

### Console
| File | Role |
|------|------|
| `main.cpp` | Menu-driven command-line UI |

### GUI
| File | Role |
|------|------|
| `LibraryGUI.h` / `LibraryGUI.cpp` | SFML widgets and screen management |
| `main_gui.cpp` | GUI startup and font loading strategy |

### Tooling and Docs
| File | Role |
|------|------|
| `CMakeLists.txt` | Primary build configuration |
| `Makefile_complete` | Legacy two-target make workflow |
| `setup.sh` | Linux helper for installing SFML dependencies |
| `README.md` | Main project documentation |
| `README_GUI.md` | GUI-specific guide |
| `QUICKSTART.md` | Build/run checklist |
| `GUI_ARCHITECTURE.md` | GUI design notes |

## 5. Data Model

Each `Book` stores:
- `isbn`
- `copyId`
- `title`
- `author`
- `year`
- `isAvailable`

The ordering relation is:
1. lower ISBN first
2. if ISBN matches, lower copy ID first

That ordering keeps duplicate copies stable inside the tree while preserving sorted traversal.

## 6. Persistence Format

`library.dat` is currently a comma-separated text file with this layout per line:

```text
isbn,title,author,year,availability
```

Notes:
- `availability` is stored as `1` or `0`.
- `copyId` is **not** stored directly.
- Copy IDs are reconstructed during loading by counting how many copies of the same ISBN have already been inserted.

## 7. Build and Deployment

### CMake
```bash
cmake -S . -B build
cmake --build build
```

### Console-only CMake build
```bash
cmake -S . -B build -DBUILD_GUI_APP=OFF
cmake --build build
```

### Install/package
```bash
cmake --install build --prefix ./dist
cd build && cpack
```

### Legacy make workflow
```bash
make -f Makefile_complete all
```

## 8. GUI Runtime Notes

The GUI requires:
- SFML graphics/window/system libraries
- a readable `.ttf` or `.ttc` font
- a desktop environment capable of opening an SFML window

Font resolution order:
1. `LIBRARY_GUI_FONT` environment variable
2. `LIBRARY_GUI_DEFAULT_FONT` configured in CMake
3. common OS-specific font paths

## 9. Operational Caveats

- The console app still prints a message about initializing sample books, but the backend actually loads from `library.dat` first.
- GUI success messages are optimistic in some flows; backend failures may still print to stdout rather than surfacing detailed GUI errors.
- Search and mutation by ISBN operate on the first matching copy, not on every copy.

## 10. Recommended Next Improvements

- Persist `copyId` explicitly in `library.dat`.
- Improve GUI feedback so it reflects backend success/failure precisely.
- Add richer search filters for title and author.
- Normalize or escape commas in serialized text fields.
- Add automated tests for duplicate-copy behavior and persistence reloads.
