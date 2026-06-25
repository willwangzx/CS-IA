# Changelog

## 2026-06-25 - Smooth Red-Black Tree Animation

### GUI
- Reworked `RBTreeVisualizer` playback so consecutive trace snapshots animate with eased interpolation instead of jumping directly between layouts.
- Matched real tree nodes by ISBN and copy ID during playback, allowing rotations and structural changes to move nodes continuously across frames.
- Added fade/scale transitions for inserted and removed nodes, plus progress-preserving pause/resume behavior.

## 2026-06-23 - Red-Black Tree Visualization

### GUI
- Added a `Red-Black Tree View` screen to the SFML GUI with operation inputs, playback controls, a tree canvas, timeline, selected-node details, and invariant checks.
- Added `RBTreeVisualizer` to keep visualization drawing and animation state separate from the main GUI screen router.

### Backend
- Added Red-Black Tree visualization snapshots and traced insert/delete steps for comparisons, insertions, deletion targets, successors, transplants, rotations, recoloring, root-black enforcement, and completion.
- Added `LibraryManagementSystem` traced add/remove operations that mutate the real persisted catalog while returning animation traces.

### Testing
- Added unit coverage for tree snapshots, traced inserts, traced deletes, and traced library add/remove operations.

## 2026-06-23 - Documentation Consistency

### Documentation
- Aligned the IA draft, README, project overview, quickstart, GUI guides, and benchmark report with the current duplicate-copy behavior.
- Updated persistence descriptions to use the current `library.dat` snapshot plus `library.dat.journal` write-ahead journal model.
- Removed outdated documentation claims about console startup seeding, non-persistent copy IDs, save-on-every-mutation behavior, and linear ISBN lookup as current behavior.
- Added a Git attribute rule so exported PDF files are treated as binary artifacts.

## 2026-05-12 - Bug Fixes

### RedBlackTree
- **Header compile failure**: Removed accidental editor connection text before the header guard in `RedBlackTree.h`, which broke every translation unit including the tree.
- **Generic header boundary**: Replaced the unnecessary `Book.h` include with `<iostream>` so `RedBlackTree` remains independent from the `Book` model while still supporting its display traversal helper.
- **Sentinel reset**: `clear()` now restores the sentinel node's child links and color as well as its parent pointer.

### Testing
- **`library_unit_tests`**: Added focused assertions for `lowerBound()` and `successor()`, covering the helpers used by optimized ISBN lookups in `LibraryManagementSystem`.

## 2026-05-11 — Performance Optimizations

### I/O Throughput
- **`saveToFile()`**: Replaced per-record `std::endl` (flush per book) with `'\n'` + single `flush()` before close. Eliminates N-1 unnecessary flushes during full catalog save.
- **`recordChange()`**: Replaced `std::endl` with `'\n'` before explicit `flush()`. Removes redundant double-flush on every journal append.

### RedBlackTree
- **`search()`**: Restructured to use only `operator<` for both navigation and equality detection instead of mixing `operator==` and `operator<`. Avoids redundant comparisons for non-matching nodes.

### Copy-ID Cache Fallback
- **`getNextCopyId()`**: Replaced full `inorderTraversal` O(n) scan with `lowerBound()` + `successor()` iteration (O(log n + k)) for the cache-miss fallback path.

### Code Quality
- **`writeCsvField`**: Extracted from duplicated lambda inside `Book::serialize()` and anonymous namespace in `LibraryManagementSystem.cpp` into a single `inline` function in `Book.h`.

## 2026-05-11 — Bug Fixes

### Persistence
- **Threshold compaction timing**: `recordChange()` now only writes/counts journal entries. `addBook()`, `removeBook()`, `checkoutBook()`, and `returnBook()` call `compactIfThresholdReached()` after applying their in-memory mutation, so the snapshot includes the operation that reaches `COMPACT_THRESHOLD`.

### Testing
- **`library_unit_tests`**: Added a regression test for the 1000th journal operation to confirm threshold compaction persists the triggering mutation across reload.

## 2026-05-10 — Performance Optimizations (TODO.md batch 2)

### RedBlackTree
- **`lowerBound(const T& key) const`**: New public method. Standard BST lower_bound in O(log n) — finds first node whose data is not less than key. Returns `nullptr` (never nil sentinel) when all elements are less than key.
- **`successor(RBNode<T>* node) const`**: New public method. Returns in-order successor in O(log n) amortized. Returns `nullptr` when node is the maximum or when passed nil/nullptr.

### ISBN Lookup Optimization
- **`findBookNode(isbn)`**: Rewritten from `findFirst` (O(n) in-order scan) to `lowerBound(Book(isbn))` (O(log n)).
- **`findAvailableBookNode(isbn)`**: Rewritten to `lowerBound` + `successor` iteration over matching ISBN copies only (O(log n + k) instead of O(n)).
- **Journal replay exact-match lookups**: Three `findFirst` calls in REMOVE/CHECKOUT/RETURN replaced with `search(Book(isbn, "", "", 0, true, copyId))` (O(log n) via `operator==` which compares isbn+copyId).
- **Old-format RETURN in journal replay**: Replaced inline `findFirst` lambda with `lookupIsbn(isbn, false)`.

### Copy-ID Cache
- **`nextCopyIdByIsbn`**: New `unordered_map<int,int>` member. Makes `getNextCopyId()` O(1) average. Populated during `loadFromFile()`, `replayJournal()`, and `addBook()`. Falls back to full tree scan on cache miss (for backward compatibility).

### Single-Scan Checkout/Return
- **`IsbnLookup` struct**: `{RBNode<Book>* matchingCopy; bool isbnExists;}` — captures both "found any ISBN?" and "found matching copy?" in one traversal.
- **`lookupIsbn(isbn, wantAvailable)`**: Private helper using `lowerBound` + `successor` for single-scan lookup.
- **`checkoutBook()`**: Uses `lookupIsbn(isbn, true)` instead of `findAvailableBookNode` + `findBookNode` fallback (was 2 scans, now 1).
- **`returnBook()`**: Uses `lookupIsbn(isbn, false)` instead of inline `findFirst` + `findBookNode` fallback.

### Journal Write Throughput
- **Persistent journal stream**: `std::ofstream journalStream` member, lazy-opened on first write, flushed per entry. Replaces per-mutation open/close. Handles write errors by closing and clearing stream state.
- **Stream lifecycle**: Closed in destructor, `compactSave()`, and `loadFromFile()` (before journal file deletion or directory change).

### Threshold-Based Compaction
- **`COMPACT_THRESHOLD = 1000`**: `recordChange()` increments `journalOpsSinceCompact`; public mutations trigger `compactSave()` after applying the journaled change when the threshold is reached.
- Counter reset in `compactSave()` and `loadFromFile()`.

### Benchmark
- **`lms_bruteforce_test`**: Added `groupSize` CLI parameter (5th arg, default 10). Controls `uniqueIsbns = count / groupSize`. Use `1` for all-unique ISBNs, `count` for all-same-ISBN.

## 2026-05-10 — Bug Fixes (TODO.md batch)

### Build System
- **CMakeLists.txt**: Updated all source file paths to use `src/` prefix. Changed include directory to `${CMAKE_CURRENT_SOURCE_DIR}/src`. Fixes `Cannot find source file` errors.
- **Makefile**: Updated source/header paths to `src/`. Changed C++ standard from C++11 to C++17 to match project requirements.
- **Makefile_complete**: Updated source/header paths to `src/`.

### Persistence
- **Book::serialize()**: Now writes `copyId` as a CSV field (6-field format: `isbn,copyId,title,author,year,available`).
- **loadFromFile()**: Parses new 6-field format with copyId preservation. Falls back to `getNextCopyId()` for old 5-field rows.
- **Journal entries**: `ADD`/`REMOVE`/`CHECKOUT`/`RETURN` now include `copyId`. `replayJournal()` uses exact copy matching when available, with backward-compatible fallback for old-format entries.
- **Title/Author sanitization**: `addBook()` strips `\n` and `\r` from title and author before writing to CSV/journal, preventing multi-row records in persistence files.

### Data Integrity
- **saveToFile()**: Reordered to try `std::rename()` first, only `std::remove()` + retry on failure. Prevents data loss if rename fails after delete.
- **RedBlackTree**: Copy constructor and copy assignment operator explicitly deleted to prevent accidental shallow copies that would cause double-free/use-after-free.

### Console App
- **main.cpp**: Removed automatic seeding of 5 sample books on every launch. Prevents duplicate records accumulating across runs.
