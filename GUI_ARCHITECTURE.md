# GUI Architecture and Screen Layout

## Class Hierarchy

```
LibraryGUI (Main Controller)
│
├── LibraryManagementSystem (Backend)
│   └── RedBlackTree<Book> (Data Structure)
│       └── Book (Data Model)
│
├── UI Components
│   ├── Button
│   ├── InputBox
│   ├── ScrollableList
│   └── MessageBox
│
└── Screens
    ├── MAIN_MENU
    ├── ADD_BOOK
    ├── REMOVE_BOOK
    ├── SEARCH_BOOK
    ├── CHECKOUT_BOOK
    ├── RETURN_BOOK
    └── VIEW_ALL
```

## Screen Layouts

### Main Menu Screen
```
┌────────────────────────────────────────────────────────┐
│                                                        │
│  ████████  Library Management System  ████████        │ ← Header Bar
│                                                        │
├────────────────────────────────────────────────────────┤
│                                                        │
│                                                        │
│              ┌──────────────────────┐                 │
│              │     Add Book         │                 │
│              └──────────────────────┘                 │
│                                                        │
│              ┌──────────────────────┐                 │
│              │    Remove Book       │                 │
│              └──────────────────────┘                 │
│                                                        │
│              ┌──────────────────────┐                 │
│              │    Search Book       │                 │
│              └──────────────────────┘                 │
│                                                        │
│              ┌──────────────────────┐                 │
│              │   Checkout Book      │                 │
│              └──────────────────────┘                 │
│                                                        │
│              ┌──────────────────────┐                 │
│              │    Return Book       │                 │
│              └──────────────────────┘                 │
│                                                        │
│              ┌──────────────────────┐                 │
│              │   View All Books     │                 │
│              └──────────────────────┘                 │
│                                                        │
└────────────────────────────────────────────────────────┘
```

### Add Book Screen
```
┌────────────────────────────────────────────────────────┐
│  ████████  Library Management System  ████████        │
├────────────────────────────────────────────────────────┤
│                                                        │
│                    Add New Book                        │
│                                                        │
│  ISBN:   ┌────────────────────────────────┐          │
│          │ Enter ISBN                     │          │
│          └────────────────────────────────┘          │
│                                                        │
│  Title:  ┌────────────────────────────────┐          │
│          │ Enter Title                    │          │
│          └────────────────────────────────┘          │
│                                                        │
│  Author: ┌────────────────────────────────┐          │
│          │ Enter Author                   │          │
│          └────────────────────────────────┘          │
│                                                        │
│  Year:   ┌────────────────────────────────┐          │
│          │ Enter Year                     │          │
│          └────────────────────────────────┘          │
│                                                        │
│                                                        │
│  ┌──────────┐              ┌──────────┐              │
│  │   Back   │              │ Add Book │              │
│  └──────────┘              └──────────┘              │
│                                                        │
│          ┌────────────────────────┐                   │
│          │  Success Message!      │ ← Message Box    │
│          └────────────────────────┘                   │
└────────────────────────────────────────────────────────┘
```

### Search/View Screen
```
┌────────────────────────────────────────────────────────┐
│  ████████  Library Management System  ████████        │
├────────────────────────────────────────────────────────┤
│                                                        │
│                    Search Book                         │
│                                                        │
│  ISBN:   ┌────────────────────────────────┐          │
│          │ Enter ISBN to search           │          │
│          └────────────────────────────────┘          │
│                                                        │
│  ┌──────────────────────────────────────────────┐    │
│  │ Results:                                     │    │
│  │ ISBN: 1001 | The Great Gatsby | ...         │    │
│  │                                              │    │
│  │                                              │    │
│  │                       ↕ Scroll               │    │
│  │                                              │    │
│  │                                              │    │
│  │                                              │    │
│  └──────────────────────────────────────────────┘    │
│                                                        │
│  ┌──────────┐              ┌──────────┐              │
│  │   Back   │              │  Search  │              │
│  └──────────┘              └──────────┘              │
└────────────────────────────────────────────────────────┘
```

### View All Books Screen
```
┌────────────────────────────────────────────────────────┐
│  ████████  Library Management System  ████████        │
├────────────────────────────────────────────────────────┤
│                                                        │
│               All Books in Library                     │
│                                                        │
│  ┌────────────────────────────────────────────────┐  │
│  │ ISBN: 1001 | The Great Gatsby | F. Scott ... │  │
│  │ ISBN: 1002 | To Kill a Mockingbird | Harp... │  │
│  │ ISBN: 1003 | 1984 | George Orwell | 1949 ... │  │
│  │ ISBN: 1004 | Pride and Prejudice | Jane ... │  │
│  │ ISBN: 1005 | The Catcher in the Rye | J.D...│  │
│  │                                                │  │
│  │                       ↕ Scroll                 │  │
│  │                                                │  │
│  │                                                │  │
│  │                                                │  │
│  │                                                │  │
│  │                                                │  │
│  └────────────────────────────────────────────────┘  │
│                                                        │
│              ┌──────────────────────┐                 │
│              │        Back          │                 │
│              └──────────────────────┘                 │
└────────────────────────────────────────────────────────┘
```

## UI Component States

### Button States
```
Normal State:        Hover State:         Active State:
┌────────────┐      ┌────────────┐      ┌────────────┐
│   Click    │  →   │   Click    │  →   │   Click    │
│  (Blue)    │      │ (Lt Blue)  │      │ (Dk Blue)  │
└────────────┘      └────────────┘      └────────────┘
```

### InputBox States
```
Inactive:            Active (Focused):
┌──────────────┐    ┌──────────────┐
│ Placeholder  │    │ Text input|  │ ← Blinking cursor
│  (Gray)      │    │  (Blue)      │ ← Blue border
└──────────────┘    └──────────────┘
```

## Event Flow

### Add Book Flow
```
User Action                System Response
───────────                ───────────────
1. Click "Add Book"    →   Load Add Book Screen
2. Click ISBN field    →   Activate input (blue border)
3. Type "2001"         →   Display characters
4. Click Title field   →   Move focus to Title
5. Type "Clean Code"   →   Display characters
6. ... (Author, Year)  →   Continue input
7. Click "Add Book"    →   Validate inputs
                       →   Add to Red-Black Tree
                       →   Show success message
                       →   Clear input fields
8. Click "Back"        →   Return to Main Menu
```

### Search Flow
```
User Action                System Response
───────────                ───────────────
1. Click "Search Book" →   Load Search Screen
2. Enter ISBN          →   Accept input
3. Click "Search"      →   Query Red-Black Tree
                       →   O(log n) search
                       →   Display result in list
                       →   Show message box
```

## Color Palette

```
Primary Colors:
├── Background:    RGB(245, 245, 245) - Light Gray
├── Header:        RGB(70, 130, 180)  - Steel Blue
├── Button Normal: RGB(70, 130, 180)  - Steel Blue
├── Button Hover:  RGB(100, 149, 237) - Cornflower Blue
└── Button Active: RGB(65, 105, 225)  - Royal Blue

Text Colors:
├── Header Text:   RGB(255, 255, 255) - White
├── Body Text:     RGB(0, 0, 0)       - Black
└── Placeholder:   RGB(150, 150, 150) - Medium Gray

Border Colors:
├── Normal:        RGB(100, 100, 100) - Dark Gray
└── Focus:         RGB(70, 130, 180)  - Steel Blue
```

## Window Dimensions

```
Main Window:       900 x 700 pixels
Header Bar:        900 x 80 pixels
Content Area:      900 x 620 pixels

Button Standard:   300 x 50 pixels
Button Small:      150 x 50 pixels
Input Box:         400 x 40 pixels
Scrollable List:   800 x 450 pixels
Message Box:       400 x 80 pixels
```

## Font Specifications

```
Title (Header):    32pt, Bold, White
Screen Title:      24pt, Bold, Black
Button Text:       18pt, Regular, White
Label Text:        18pt, Regular, Black
Input Text:        16pt, Regular, Black
List Text:         14pt, Regular, Black
Message Text:      16pt, Regular, Black
```

## Animation & Effects

```
Hover Animation:
  Duration: Instant (on mouse move)
  Effect: Color transition

Cursor Blink:
  Duration: 0.5 seconds on/off
  Effect: Show/hide pipe character "|"

Message Box:
  Duration: 3 seconds
  Effect: Fade out (auto-hide)

Scroll:
  Duration: Instant (on wheel)
  Effect: Shift visible items
```

## Memory Layout

```
LibraryGUI Object
├── window (SFML RenderWindow)
├── library (LibraryManagementSystem)
│   └── bookTree (RedBlackTree<Book>)
│       └── nodes (dynamic allocation)
├── font (SFML Font)
├── mainMenuButtons (vector of unique_ptr<Button>)
├── inputBoxes (vector of unique_ptr<InputBox>)
├── bookList (unique_ptr<ScrollableList>)
├── messageBox (unique_ptr<MessageBox>)
├── backButton (unique_ptr<Button>)
└── submitButton (unique_ptr<Button>)
```

This architecture ensures:
- Clean separation of concerns
- Efficient memory management with smart pointers
- Responsive user interface
- Scalable design for future enhancements
