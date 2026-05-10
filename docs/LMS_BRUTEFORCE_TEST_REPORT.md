# LibraryManagementSystem Brute-Force Test Report

Generated on 2026-04-27.

## Purpose

This test directly exercises `LibraryManagementSystem` through its public interface instead of testing the underlying `RedBlackTree` in isolation. The goal is to measure real backend behavior with persistence enabled, including the cost of repeatedly rewriting `library.dat`.

The benchmark uses these public methods:

- `addBook`
- `findBook`
- `checkoutBook`
- `returnBook`
- `forEachBook`
- `saveToFile`
- `removeBook`
- `loadFromFile`

The test runs in a temporary working directory, so the benchmark creates and rewrites its own isolated `library.dat` and does not modify the project catalog.

## Test Setup

Command:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\run_lms_bruteforce.ps1 -BuildDir build-tests -Configuration Release -OutputDir docs
```

Build configuration:

- Compiler: MSVC via CMake / Visual Studio 17 2022
- C++ standard: C++17
- GUI target disabled
- Benchmark target: `lms_bruteforce_test`

Dataset pattern:

- Counts tested: `100`, `500`, `1000`, `2000`
- Unique ISBN groups: `count / 10`
- Each ISBN therefore has about 10 physical copies
- `findBook` uses a 75% hit / 25% miss lookup mix
- `checkoutBook` runs `count / 2` operations
- `returnBook` runs `count / 4` operations
- `removeBook` removes one copy from each ISBN group

Raw CSV output is stored in [lms_bruteforce_results.csv](lms_bruteforce_results.csv).

## Result Summary

| Count | Unique ISBNs | Final Books | Available | Unavailable | `library.dat` Bytes | Explicit Save Bytes |
|---:|---:|---:|---:|---:|---:|---:|
| 100 | 10 | 90 | 65 | 25 | 3,057 | 3,377 |
| 500 | 50 | 450 | 325 | 125 | 15,650 | 17,330 |
| 1000 | 100 | 900 | 650 | 250 | 31,403 | 34,780 |
| 2000 | 200 | 1800 | 1300 | 500 | 63,816 | 70,680 |

The final book count is lower than the insertion count because the benchmark removes one copy from every ISBN group after checkout/return testing.

## Operation Timings

| Count | Operation | Ops | Seconds | Ops/s |
|---:|---|---:|---:|---:|
| 100 | addBook | 100 | 0.119771 | 834.93 |
| 100 | findBook | 100 | 0.000009 | 11,494,252.87 |
| 100 | checkoutBook | 50 | 0.066195 | 755.35 |
| 100 | returnBook | 25 | 0.043460 | 575.24 |
| 100 | forEachBook | 1 | 0.000002 | 454,545.45 |
| 100 | saveToFile | 1 | 0.002142 | 466.94 |
| 100 | removeBook | 10 | 0.009653 | 1,035.93 |
| 100 | loadFromFile | 1 | 0.000129 | 7,757.95 |
| 500 | addBook | 500 | 1.092603 | 457.62 |
| 500 | findBook | 500 | 0.000202 | 2,470,355.73 |
| 500 | checkoutBook | 250 | 0.790413 | 316.29 |
| 500 | returnBook | 125 | 0.473213 | 264.15 |
| 500 | forEachBook | 1 | 0.000006 | 166,666.67 |
| 500 | saveToFile | 1 | 0.002299 | 435.07 |
| 500 | removeBook | 50 | 0.155754 | 321.02 |
| 500 | loadFromFile | 1 | 0.000620 | 1,613.16 |
| 1000 | addBook | 1000 | 2.728927 | 366.44 |
| 1000 | findBook | 1000 | 0.000843 | 1,185,536.46 |
| 1000 | checkoutBook | 500 | 2.118880 | 235.97 |
| 1000 | returnBook | 250 | 1.080240 | 231.43 |
| 1000 | forEachBook | 1 | 0.000012 | 81,300.81 |
| 1000 | saveToFile | 1 | 0.004056 | 246.58 |
| 1000 | removeBook | 100 | 0.417188 | 239.70 |
| 1000 | loadFromFile | 1 | 0.002174 | 459.92 |
| 2000 | addBook | 2000 | 8.414046 | 237.70 |
| 2000 | findBook | 2000 | 0.005025 | 398,049.56 |
| 2000 | checkoutBook | 1000 | 7.101286 | 140.82 |
| 2000 | returnBook | 500 | 3.612018 | 138.43 |
| 2000 | forEachBook | 1 | 0.000046 | 21,929.82 |
| 2000 | saveToFile | 1 | 0.008327 | 120.10 |
| 2000 | removeBook | 200 | 1.371532 | 145.82 |
| 2000 | loadFromFile | 1 | 0.008877 | 112.65 |

## Analysis

`addBook` is the dominant cost. At 100 records it completed at about 835 ops/s, but at 2000 records it dropped to about 238 ops/s. This happens because each call does two expensive things: it scans the tree to calculate the next copy ID for that ISBN, then rewrites the entire `library.dat` file.

`checkoutBook`, `returnBook`, and `removeBook` show the same pattern. They are not just tree updates; each successful mutation also calls `saveToFile("library.dat")`, so the cost grows with catalog size. At 2000 inserted records, `checkoutBook` needed 7.10 seconds for 1000 operations.

`findBook` is much faster because it does not write to disk. However, it still uses `findFirst`, which performs an in-order traversal rather than a direct ISBN-keyed tree search. The benchmark stays fast at 2000 records, but this operation is still effectively linear in the current design.

`forEachBook`, single `saveToFile`, and `loadFromFile` scale roughly with the number of records. That is expected: each of these operations touches the whole catalog once.

The serialized file size grows linearly. The explicit save after 2000 records is 70,680 bytes, about 35 bytes per record for this generated dataset.

## Scaling Implication

The previous red-black-tree-only stress test handled 10,000,000 inserts successfully because it measured the tree structure directly. This brute-force test is different: it uses `LibraryManagementSystem::addBook`, which rewrites the full data file on every insertion.

Using the 2000-record measurement as a rough lower-bound model, scaling `addBook` to 10,000,000 records with the current persistence design would be unrealistic. The repeated full-file saves imply quadratic total file output. With about 35 bytes per record, 10,000,000 repeated inserts would imply petabyte-scale cumulative file writing, before counting checkout/return/remove operations.

So the current implementation is acceptable for small educational datasets, but it is not designed for very large catalogs through the public LMS mutation API.

## Conclusions

- The public LMS interface works correctly under the tested brute-force scenarios.
- The final catalog counts match the expected workflow state.
- Persistence is the main performance bottleneck.
- The tree itself is not the limiting factor in this public-interface benchmark.
- `findBook` is faster than mutating operations, but still uses linear predicate traversal.
- Direct `LibraryManagementSystem` testing at `1e7` scale is not practical with the current save-on-every-mutation design.

## Recommended Improvements

- Avoid rewriting `library.dat` after every single mutation during bulk operations.
- Add a bulk import mode, for example `addBook(..., autosave=false)` or `beginBatch/endBatch`.
- Store books by ISBN in a structure that supports direct ISBN lookup instead of predicate traversal.
- Persist `copyId` in the file format so loading does not need to regenerate copy IDs by scanning existing records.
- Replace ad hoc CSV parsing if titles/authors may contain commas.
