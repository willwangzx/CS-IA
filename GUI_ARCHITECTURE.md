# GUI Architecture

This document summarizes the GUI structure as it exists in the current codebase.

## High-Level Structure

```text
main_gui.cpp
  -> discovers a usable font
  -> constructs LibraryGUI
  -> starts the event/render loop

LibraryGUI
  -> owns the SFML window
  -> owns the LibraryManagementSystem backend
  -> manages screen state and UI widgets

LibraryManagementSystem
  -> loads/saves library.dat
  -> stores Book objects in RedBlackTree<Book>

main.cpp only
  -> seeds five sample books after backend construction
```

## Main Objects

### `LibraryGUI`
Responsible for:
- window creation
- screen switching
- widget initialization per screen
- event routing
- render dispatch
- hover-state updates
- holding the backend instance

### `Button`
A reusable clickable widget with:
- normal color
- hover color
- active color

### `InputBox`
A lightweight text input widget with:
- focus state
- placeholder rendering
- simple character entry
- blinking cursor effect

### `ScrollableList`
Used for search results and the full catalog view.

Characteristics:
- stores rendered text rows
- supports vertical mouse-wheel scrolling
- displays up to 10 items at once

### `MessageBox`
A temporary notification overlay.

Characteristics:
- fixed position near the lower part of the window
- configurable duration
- auto-hides after timeout

## Screen State Machine

`LibraryGUI` uses this enum:

```cpp
enum class Screen {
    MAIN_MENU,
    ADD_BOOK,
    REMOVE_BOOK,
    SEARCH_BOOK,
    CHECKOUT_BOOK,
    RETURN_BOOK,
    VIEW_ALL
};
```

Each screen has:
- an initializer
- an event handler
- a renderer

## Backend Integration

The GUI talks directly to `LibraryManagementSystem`.

Current backend interactions:
- `addBook(isbn, title, author, year)`
- `removeBook(isbn)`
- `findBook(isbn)`
- `checkoutBook(isbn)`
- `returnBook(isbn)`
- `forEachBook(...)`

Important behavioral detail:
- operations by ISBN act on the **first relevant copy**, not on all copies

## Runtime Font Bootstrap

`main_gui.cpp` is responsible for font selection before `LibraryGUI::run()` starts.

Resolution order:
1. `LIBRARY_GUI_FONT`
2. compile-time default from `LIBRARY_GUI_DEFAULT_FONT`
3. platform-specific fallback paths

This keeps font setup flexible across Linux, macOS, and Windows.

## Rendering Model

The GUI uses a straightforward immediate-style redraw loop:
- poll SFML events
- route events based on the current screen
- update hover/message state
- clear and redraw the full frame
- display the rendered frame

## Current Data Flow Examples

### Add Book
```text
User enters ISBN/title/author/year
  -> Add Book screen parses values
  -> LibraryManagementSystem::addBook(...)
  -> backend assigns next copy ID for the ISBN
  -> backend saves library.dat
  -> GUI shows a temporary success message
```

### View All
```text
User opens View All Books
  -> GUI rebuilds ScrollableList from forEachBook(...)
  -> RedBlackTree inorder traversal produces sorted rows
  -> rows display ISBN/title/author/year/status
```

### Search
```text
User enters ISBN
  -> GUI calls findBook(isbn)
  -> first matching copy is returned
  -> result list shows one row or a not-found row
```

## Practical Limitations

- The GUI currently mixes presentation with backend calls in the same class.
- Search results are single-row even when multiple copies exist for one ISBN.

## Suggested Refactors

- Add a copy-aware search/listing mode for duplicate ISBNs.
- Move screen-specific form validation into helpers.
