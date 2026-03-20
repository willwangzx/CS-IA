# Quick Start Guide

Use this guide when you want to build the current version of the project quickly and correctly.

## 1. Choose a Build Path

### Recommended: CMake
Use this if you want the most up-to-date build flow.

```bash
cmake -S . -B build
cmake --build build
```

This always builds the console app and builds the GUI app only if SFML is available.

### Legacy: Makefile
Use this only if you already have the expected compiler and, for the GUI, SFML linker setup.

```bash
make -f Makefile_complete all
```

## 2. Prerequisites

### Console app
- C++17-compatible compiler
- CMake 3.16+ if using the recommended path

### GUI app
- SFML graphics/window/system libraries
- A GUI-capable desktop session
- A readable font file (`.ttf` or `.ttc`)

## 3. Build Commands

### Build both apps when possible
```bash
cmake -S . -B build
cmake --build build
```

### Build console only
```bash
cmake -S . -B build -DBUILD_GUI_APP=OFF
cmake --build build
```

### Build with a fixed GUI font path
```bash
cmake -S . -B build -DLIBRARY_GUI_DEFAULT_FONT=/absolute/path/to/font.ttf
cmake --build build
```

## 4. Run Commands

### Console
```bash
./build/library_system
```

### GUI
```bash
./build/library_system_gui
```

If the GUI cannot find a font automatically:

```bash
LIBRARY_GUI_FONT=/absolute/path/to/font.ttf ./build/library_system_gui
```

## 5. First Run Expectations

### No built-in starter catalog guarantee
The backend now loads from `library.dat`. If that file does not exist or is empty, the library starts empty.

### Data is saved automatically
Successful add, remove, checkout, and return operations rewrite `library.dat`.

### Duplicate copies are allowed
Adding the same ISBN multiple times creates multiple copies instead of rejecting the insert.

## 6. Typical Console Flow

1. Start `./build/library_system`
2. Choose `1` to add a book
3. Enter ISBN, title, author, and year
4. Choose `6` to display the catalog
5. Exit with `7`

## 7. Typical GUI Flow

1. Start `./build/library_system_gui`
2. Open **Add Book**
3. Enter ISBN, title, author, and year
4. Submit the form
5. Open **View All Books** to inspect the saved records

## 8. Troubleshooting

### GUI executable was not built
SFML was probably not found during CMake configuration.

Reconfigure after installing SFML:

```bash
cmake -S . -B build
cmake --build build
```

### GUI says it cannot load a font
Provide a font explicitly:

```bash
LIBRARY_GUI_FONT=/absolute/path/to/font.ttf ./build/library_system_gui
```

### Want to install dependencies on Linux
A helper script is available:

```bash
./setup.sh
```

### Want installable artifacts
```bash
cmake --install build --prefix ./dist
cd build && cpack
```
