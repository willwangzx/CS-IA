# Library Management System - Project Overview

This project is a C++17 library manager with a Red-Black Tree backend, persistent file storage, duplicate-copy handling, a console UI, and an optional SFML desktop GUI.

## 1. System Overview

### Core Capabilities
- Store books in a balanced Red-Black Tree.
- Load the catalog from `library.dat`, replay `library.dat.journal`, and compact successful changes back into the snapshot.
- Support multiple copies of the same ISBN.
- Track availability on a per-copy basis.
- Expose both console and GUI workflows over the same backend.
- Visualize and animate the real Red-Black Tree used by the GUI catalog.

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
| `RBTreeVisualizer` | SFML tree canvas, animation timeline, and invariant inspector |

## 3. Important Recent Changes

### Persistent Storage
The system calls `loadFromFile("library.dat")` in the `LibraryManagementSystem` constructor, then replays `library.dat.journal` if pending journal entries exist. Successful mutations append write-ahead journal entries. The backend compacts the journal into a full `library.dat` snapshot when the journal reaches the compaction threshold or when the backend is destroyed.

The console and GUI front ends now start from the same persisted catalog. Neither front end inserts sample books automatically.

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

### Red-Black Tree Visualization
`RedBlackTree` now exposes snapshot and trace structures for visualization without exposing its private root or sentinel pointers. The GUI `Red-Black Tree View` screen uses those snapshots to draw the real catalog tree and animate traced add/remove operations, including comparisons, successor selection, transplants, rotations, recoloring, and invariant checks.

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
| `RBTreeVisualizer.h` / `RBTreeVisualizer.cpp` | Red-Black Tree visualizer controls, drawing, and playback |
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
isbn,copyId,title,author,year,availability
```

Notes:
- `availability` is stored as `1` or `0`.
- `copyId` is stored directly so duplicate physical copies keep stable identities across reloads.
- Title and author fields are written as CSV fields, so commas and quotes are escaped.
- Older five-field rows without `copyId` are still loaded by assigning copy IDs during import.
- `library.dat.journal` records `ADD`, `REMOVE`, `CHECKOUT`, and `RETURN` entries with copy IDs for recovery before compaction.

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

The `Red-Black Tree View` GUI screen is not a sandbox: animated add/remove actions write the normal journal entries and mutate the same persisted catalog used by the rest of the app.

## 9. Operational Caveats

- GUI messages distinguish common failure cases, but detailed backend errors may still be printed to stdout.
- Search and mutation by ISBN operate on the first matching copy, not on every copy.
- `compactSave()` still rewrites the full `library.dat` snapshot, so compaction is `O(n)` even though ordinary mutations append to the journal.

## 10. Recommended Next Improvements

- Improve GUI feedback so it reflects backend success/failure precisely.
- Add richer search filters for title and author.
- Add copy-specific GUI operations so users can select an exact copy rather than the first relevant copy.
- Add automated tests for GUI-level duplicate-copy workflows.
- Add a sandbox mode if classroom demonstrations should avoid changing `library.dat`.
- Re-run and refresh benchmark documentation after the journal and lookup optimizations.
