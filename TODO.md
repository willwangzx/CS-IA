# TODO

This list records confirmed bugs and build issues found in the current repository. When an item is fixed, remove it from this list and record the fix in the changelog.

## Bugs and Build Issues

(All previously recorded issues have been resolved — see changelog.md.)

## Performance Optimization Suggestions

- ~~Prioritize ISBN lookups~~ — Done 2026-05-10. Added `lowerBound()` and `successor()` to `RedBlackTree`, rewrote `findBookNode()`, `findAvailableBookNode()`, and journal replay exact-match lookups to use O(log n) tree search instead of O(n) `findFirst()` scans.
- ~~Avoid full-tree copy ID scans in `getNextCopyId()`~~ — Done 2026-05-10. Added `nextCopyIdByIsbn` cache (`unordered_map<int,int>`), populated during `loadFromFile()`, `replayJournal()`, and `addBook()`. Falls back to tree scan if cache miss.
- ~~Reduce repeated scans during checkout/return~~ — Done 2026-05-10. Added `IsbnLookup` struct and `lookupIsbn()` helper that captures both `isbnExists` and `matchingCopy` in a single O(log n + k) traversal.
- ~~Improve journal write throughput~~ — Done 2026-05-10. Persistent `std::ofstream` member opened lazily and flushed per-write instead of open/close per mutation. Handles error recovery (close+clear on write failure).
- ~~Make compaction threshold-based~~ — Done 2026-05-10, fixed 2026-05-11. Added `COMPACT_THRESHOLD = 1000` journal operations; public mutations now compact only after the journaled in-memory change has been applied.
- ~~Add benchmark coverage for optimized paths~~ — Done 2026-05-10. Added `groupSize` CLI parameter to `lms_bruteforce_test` for controlling duplicate-ISBN ratio (default 10, use 1 for all-unique, N for all-same).

## Follow-up Tests

- After recording this file, run `rg --files | rg -i "todo|changelog"` to confirm `TODO.md` is present.
- After fixing build paths, verify `cmake -S . -B build -DBUILD_GUI_APP=OFF` and `cmake --build build`.
- After fixing copy persistence and journal replay, add tests for duplicate ISBNs, multi-copy checkout/remove/return, save-and-reload identity, and journal recovery without `copyId` drift.
