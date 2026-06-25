# CLAUDE.md

## Project

C++17 Library Management System - an IB CS Internal Assessment project. Manages a physical book collection with a Red-Black Tree backend, persistent file storage, duplicate-ISBN copy support, and an animated SFML Red-Black Tree visualizer. Two frontends: console (CLI menu) and SFML 3 desktop GUI.

## Build

**Primary: CMake 3.21+**

```bash
# Configure & build (all targets)
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build

# Console-only (no SFML dependency)
cmake -S . -B build -DBUILD_GUI_APP=OFF

# Run tests
cd build && ctest --output-on-failure
```

**PowerShell test scripts** (in `scripts/`):
- `run_tests.ps1` - build and run unit tests via CTest
- `run_deep_tests.ps1` - unit tests + RBT stress test (default 10M elements)
- `run_stress_test.ps1` - targeted RBT stress test with custom parameters
- `run_lms_bruteforce.ps1` - LMS end-to-end benchmark (default 100/500/1000/2000 records)

**Legacy Makefiles**: `Makefile` (console, C++11) and `Makefile_complete` (console+GUI, C++17, links SFML)

## Architecture

```text
main_gui.cpp / main.cpp          Entry points
        |
LibraryGUI / console menu        UI layer (SFML / stdin)
        |
LibraryManagementSystem          Business logic + persistence
        |
RedBlackTree<Book>               Ordered storage, snapshots, traced add/remove
        |
Book                             Data model (isbn, copyId, title, author, year, available)
```

### Key source files

| File | Role |
|------|------|
| `src/Book.h` / `src/Book.cpp` | Data model with CSV serialization. Comparison sorts by ISBN then copyId. |
| `src/RedBlackTree.h` | Generic header-only RBT template. Sentinel nil node, CLRS-style fixup, no direct `Book` dependency. `findFirst(predicate)` is O(n); `lowerBound()`/`successor()` support optimized LMS ISBN scans. Also exposes visualization snapshots and traced insert/delete steps without exposing private root/sentinel pointers. |
| `src/LibraryManagementSystem.h` / `src/LibraryManagementSystem.cpp` | Backend API: add/remove/checkout/return with copy-ID awareness, traced tree add/remove operations, and visualization snapshot access. Write-ahead journal (`library.dat.journal`) + compacted snapshot (`library.dat`) persistence. |
| `src/LibraryGUI.h` / `src/LibraryGUI.cpp` | SFML GUI with Button, InputBox, ScrollableList, MessageBox widgets. Screen state machine (8 screens). |
| `src/RBTreeVisualizer.h` / `src/RBTreeVisualizer.cpp` | SFML Red-Black Tree visualizer screen: operation controls, tree canvas, smooth interpolated animation timeline, selected node panel, and invariant checks. |
| `src/main.cpp` | Console CLI menu using the shared persisted backend; no automatic sample seeding. |
| `src/main_gui.cpp` | GUI bootstrap with cross-platform font discovery. |

### CMake targets

- `library_core` - static lib from Book + LibraryManagementSystem (alias: `LibraryManagementSystem::core`)
- `library_system` - console executable
- `library_system_gui` - SFML GUI executable (requires SFML 3)
- `library_unit_tests` - 15 unit tests, registered with CTest
- `rbtree_stress_test` - RBT stress test (up to 10M elements)
- `lms_bruteforce_test` - LMS integration benchmark

## Testing

Custom lightweight framework - no external test library. Assertions: `expect()`, `expectEqual()`. Output capture via `captureStdout()`. Test cases registered with `runCase()`.

**Unit tests** (`tests/library_unit_tests.cpp`): 15 cases covering Book model, RBT operations, RBT visualization snapshots/traces, LMS traced tree operations, LMS copy workflow, persistence round-trip, CSV edge cases, journal recovery, destructor compaction, and threshold compaction.

**Stress test** (`tests/rbtree_stress_test.cpp`): Inserts up to 10M ints (ordered/reverse/permuted patterns), validates inorder traversal + sum, optional removal phase. Timed per phase.

**Benchmark** (`tests/lms_bruteforce_test.cpp`): Full LMS public-API workload with timing per operation group. Outputs CSV results.

## Conventions

- C++17 standard
- `.gitignore` uses deny-first (allowlist) pattern - add new file types explicitly
- SFML 3 (not SFML 2) for GUI
- Persistence files: `library.dat` (compacted snapshot), `library.dat.journal` (write-ahead log)
- GUI font resolution: `LIBRARY_GUI_FONT` env var -> `LIBRARY_GUI_DEFAULT_FONT` define -> platform fallback paths
- CI: GitHub Actions matrix (ubuntu-gcc, ubuntu-clang, windows-msvc) on push/PR to `main`

## Known limitations

- `compactSave()` rewrites the entire catalog file. Threshold-based compaction (every 1000 journal ops, after the triggering mutation is applied) limits frequency, but the full rewrite is still O(n). Per-record flush eliminated - single `flush()` at end.
- Journal writes use persistent stream with explicit flush per mutation - durable but can bottleneck on very large bulk-add workloads.
- Copy-ID cache is monotonic: after removing all copies of an ISBN, re-adding starts from `oldMax + 1` rather than reusing freed IDs.
- Red-Black Tree visualizer add/remove operations mutate the real persisted catalog, not a sandbox tree.
- `findBookNode()` and related lookups use `lowerBound()` (O(log n)) instead of `findFirst()` (O(n)).
- `getNextCopyId()` cache-miss fallback now uses `lowerBound()` + `successor()` (O(log n + k)) instead of full `inorderTraversal` (O(n)).
- `search()` uses only `operator<` for tree navigation; `writeCsvField` is a shared `inline` function in `Book.h`.

## Workflow

1. record bugs that need to be fixed in todo list (TODO.md), and delete the ones that are fixed and record the fixed bugs in the changelog (changelog.md)
2. Before making any code changes, check the TODO.md file to see if there are any existing bugs that need to be fixed. If there are, prioritize fixing those bugs before adding new features or making other changes. Before applying the change, think why this approach is better than the existing one, and if it is not, then do not apply the change.
3. After making code changes, run all the tests to ensure that the changes do not break any existing functionality. If any tests fail, investigate the cause and fix the issue before proceeding.
4. After applying changes to the code, ensure that you update the documentation in this file to reflect the changes made. This includes updating any relevant sections such as architecture, testing, or known limitations.
