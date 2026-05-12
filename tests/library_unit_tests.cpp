#include "Book.h"
#include "LibraryManagementSystem.h"
#include "RedBlackTree.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

int failures = 0;
int assertions = 0;
std::vector<std::string> failedCases;

void expect(bool condition, const std::string& message) {
    ++assertions;
    if (!condition) {
        ++failures;
        std::cerr << "  FAIL: " << message << std::endl;
    }
}

template <typename T>
void expectEqual(const T& actual, const T& expected, const std::string& message) {
    ++assertions;
    if (!(actual == expected)) {
        ++failures;
        std::cerr << "  FAIL: " << message << " (expected " << expected
                  << ", got " << actual << ")" << std::endl;
    }
}

std::string captureStdout(const std::function<void()>& func) {
    std::ostringstream captured;
    std::streambuf* original = std::cout.rdbuf(captured.rdbuf());
    func();
    std::cout.rdbuf(original);
    return captured.str();
}

void removeTestDataFiles() {
    std::remove("library.dat");
    std::remove("library.dat.journal");
    std::remove("unit_roundtrip.dat");
    std::remove("unit_load_input.dat");
}

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

template <typename Func>
void runCase(const std::string& name, Func func) {
    const int failuresBefore = failures;
    std::cout << "[ RUN      ] " << name << std::endl;
    func();

    if (failures == failuresBefore) {
        std::cout << "[       OK ] " << name << std::endl;
    } else {
        failedCases.push_back(name);
        std::cout << "[  FAILED  ] " << name << std::endl;
    }
}

std::vector<int> collectTreeValues(RedBlackTree<int>& tree) {
    std::vector<int> values;
    tree.inorderTraversal([&values](int value) {
        values.push_back(value);
    });
    return values;
}

std::vector<Book> collectBooks(const LibraryManagementSystem& library) {
    std::vector<Book> books;
    library.forEachBook([&books](const Book& book) {
        books.push_back(book);
    });
    return books;
}

void testBookDefaultsAndSerialization() {
    Book defaultBook;
    expectEqual(defaultBook.getISBN(), 0, "default ISBN is zero");
    expectEqual(defaultBook.getCopyId(), 0, "default copy ID is zero");
    expectEqual(defaultBook.getTitle(), std::string(""), "default title is empty");
    expect(defaultBook.getAvailability(), "default book is available");

    Book checkedOut(1002, "1984", "George Orwell", 1949, false, 3);
    expectEqual(checkedOut.getISBN(), 1002, "constructor stores ISBN");
    expectEqual(checkedOut.getCopyId(), 3, "constructor stores copy ID");
    expectEqual(checkedOut.getTitle(), std::string("1984"), "constructor stores title");
    expectEqual(checkedOut.getAuthor(), std::string("George Orwell"), "constructor stores author");
    expectEqual(checkedOut.getYear(), 1949, "constructor stores year");
    expect(!checkedOut.getAvailability(), "constructor stores availability");

    checkedOut.setAvailability(true);
    expect(checkedOut.getAvailability(), "setAvailability updates availability");

    std::ostringstream serialized;
    checkedOut.serialize(serialized);
    expectEqual(serialized.str(), std::string("1002,3,1984,George Orwell,1949,1"),
                "serialization writes persisted fields including copyId in CSV order");

    Book quotedBook(1003, "Title, \"Quoted\"", "Author, Jr.", 2020, true, 1);
    std::ostringstream quotedSerialized;
    quotedBook.serialize(quotedSerialized);
    expectEqual(quotedSerialized.str(),
                std::string("1003,1,\"Title, \"\"Quoted\"\"\",\"Author, Jr.\",2020,1"),
                "serialization quotes CSV fields that contain commas or quotes");

    std::string display = captureStdout([&checkedOut]() {
        checkedOut.display();
    });
    expect(display.find("1002,copy 3,1984,George Orwell,1949,1") != std::string::npos,
           "display includes ISBN, copy ID, metadata, and availability");
}

void testBookOrderingAndIdentity() {
    Book firstCopy(1001, "Same ISBN", "Author", 2000, true, 1);
    Book secondCopy(1001, "Same ISBN", "Author", 2000, true, 2);
    Book otherBook(1002, "Other", "Author", 2001, true, 1);

    expect(firstCopy < secondCopy, "same-ISBN books are ordered by copy ID");
    expect(secondCopy > firstCopy, "greater-than uses copy ID after ISBN");
    expect(firstCopy < otherBook, "lower ISBN sorts before higher ISBN");
    expect(otherBook > firstCopy, "higher ISBN sorts after lower ISBN");
    expect(firstCopy == Book(1001, "Different", "Different", 2020, false, 1),
           "identity comparison ignores title, author, year, and availability");
}

void testRedBlackTreeEmptyDuplicateAndClear() {
    RedBlackTree<int> tree;
    expect(tree.isEmpty(), "new tree starts empty");
    expect(tree.search(10) == nullptr, "empty tree search returns null");

    tree.remove(10);
    expect(tree.isEmpty(), "removing from an empty tree keeps it empty");

    tree.insert(5);
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    expect(!tree.isEmpty(), "tree is not empty after inserts");
    expect(collectTreeValues(tree) == std::vector<int>({3, 5, 5, 7}),
           "tree traversal includes duplicate values in sorted order");

    RBNode<int>* duplicate = tree.findFirst([](int value) {
        return value == 5;
    });
    expect(duplicate != nullptr && duplicate->data == 5,
           "findFirst can locate a duplicated value");

    RBNode<int>* firstFive = tree.lowerBound(5);
    expect(firstFive != nullptr && firstFive->data == 5,
           "lowerBound finds the first duplicate candidate");

    RBNode<int>* secondFive = tree.successor(firstFive);
    expect(secondFive != nullptr && secondFive->data == 5,
           "successor walks duplicate values in sorted order");

    tree.clear();
    expect(tree.isEmpty(), "clear empties the tree");
    expect(collectTreeValues(tree).empty(), "cleared tree traverses as empty");
}

void testRedBlackTreeOrderingSearchAndRemoval() {
    RedBlackTree<int> tree;
    std::vector<int> values{42, 7, 19, 100, 1, 85, 13, 60, 55, 58, 57};

    for (int value : values) {
        tree.insert(value);
    }

    std::vector<int> expected = values;
    std::sort(expected.begin(), expected.end());
    expect(collectTreeValues(tree) == expected,
           "in-order traversal returns sorted values after mixed inserts");
    expect(tree.search(19) != nullptr, "search finds an inserted value");
    expect(tree.search(999) == nullptr, "search returns null for a missing value");
    expect(tree.lowerBound(56) != nullptr && tree.lowerBound(56)->data == 57,
           "lowerBound returns the next greater value for missing keys");
    expect(tree.lowerBound(101) == nullptr,
           "lowerBound returns null when every value is smaller than the key");

    RBNode<int>* firstLargeValue = tree.findFirst([](int value) {
        return value > 50;
    });
    expect(firstLargeValue != nullptr && firstLargeValue->data == 55,
           "findFirst returns the first matching value in sorted order");

    tree.remove(42);
    tree.remove(1);
    tree.remove(100);
    tree.remove(55);
    tree.remove(999);

    expect(collectTreeValues(tree) == std::vector<int>({7, 13, 19, 57, 58, 60, 85}),
           "remove handles mixed deletion cases and ignores missing values");
}

void testLibraryStartsEmptyAndReportsEmptyCatalog() {
    removeTestDataFiles();

    LibraryManagementSystem library;
    expect(collectBooks(library).empty(), "library starts empty when library.dat is absent");
    expect(library.findBook(9999) == nullptr, "missing ISBN lookup returns null");

    std::string output = captureStdout([&library]() {
        library.displayAllBooks();
    });
    expect(output.find("The library is empty.") != std::string::npos,
           "displayAllBooks reports an empty catalog");
}

void testLibraryCopyWorkflowAndMessages() {
    removeTestDataFiles();

    LibraryManagementSystem library;
    captureStdout([&library]() {
        library.addBook(2001, "Clean Code", "Robert C. Martin", 2008);
        library.addBook(2001, "Clean Code", "Robert C. Martin", 2008);
        library.addBook(2002, "The Pragmatic Programmer", "Andrew Hunt", 1999);
    });

    std::vector<Book> books = collectBooks(library);
    expectEqual(books.size(), static_cast<std::size_t>(3), "library stores multiple copies");
    expectEqual(books[0].getISBN(), 2001, "books are traversed by ISBN");
    expectEqual(books[0].getCopyId(), 1, "first copy ID starts at 1");
    expectEqual(books[1].getCopyId(), 2, "duplicate ISBN receives the next copy ID");
    expectEqual(books[2].getISBN(), 2002, "different ISBN is sorted after duplicate copies");

    std::string missingCheckout = captureStdout([&library]() {
        library.checkoutBook(9999);
    });
    expect(missingCheckout.find("not found") != std::string::npos,
           "checkout reports a missing ISBN");

    captureStdout([&library]() {
        library.checkoutBook(2001);
        library.checkoutBook(2001);
    });

    int unavailableCopies = 0;
    library.forEachBook([&unavailableCopies](const Book& book) {
        if (book.getISBN() == 2001 && !book.getAvailability()) {
            ++unavailableCopies;
        }
    });
    expectEqual(unavailableCopies, 2, "checkout updates one available copy at a time");

    std::string allCheckedOut = captureStdout([&library]() {
        library.checkoutBook(2001);
    });
    expect(allCheckedOut.find("already checked out") != std::string::npos,
           "checkout reports when every copy is unavailable");

    captureStdout([&library]() {
        library.returnBook(2001);
    });

    int availableCopies = 0;
    library.forEachBook([&availableCopies](const Book& book) {
        if (book.getISBN() == 2001 && book.getAvailability()) {
            ++availableCopies;
        }
    });
    expectEqual(availableCopies, 1, "return updates one checked-out copy at a time");

    captureStdout([&library]() {
        library.returnBook(2001);
    });
    std::string allAvailable = captureStdout([&library]() {
        library.returnBook(2001);
    });
    expect(allAvailable.find("already available") != std::string::npos,
           "return reports when every copy is already available");

    captureStdout([&library]() {
        library.removeBook(2001);
    });
    expectEqual(collectBooks(library).size(), static_cast<std::size_t>(2),
                "removeBook deletes one matching copy");

    std::string missingRemove = captureStdout([&library]() {
        library.removeBook(7777);
    });
    expect(missingRemove.find("not found") != std::string::npos,
           "removeBook reports a missing ISBN");
}

void testLibraryDisplayAndPersistence() {
    removeTestDataFiles();

    LibraryManagementSystem library;
    captureStdout([&library]() {
        library.addBook(3002, "B Title", "B Author", 2002);
        library.addBook(3001, "A Title", "A Author", 2001);
        library.addBook(3001, "A Title", "A Author", 2001);
        library.checkoutBook(3001);
    });

    std::string displayFound = captureStdout([&library]() {
        library.displayBook(3001);
    });
    expect(displayFound.find("3001,copy 1,A Title,A Author,2001,0") != std::string::npos,
           "displayBook prints the first matching copy");

    std::string displayMissing = captureStdout([&library]() {
        library.displayBook(9999);
    });
    expect(displayMissing.find("not found") != std::string::npos,
           "displayBook reports a missing ISBN");

    library.saveToFile("unit_roundtrip.dat");

    std::ifstream saved("unit_roundtrip.dat");
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(saved, line)) {
        lines.push_back(line);
    }
    expect(lines == std::vector<std::string>({
               "3001,1,A Title,A Author,2001,0",
               "3001,2,A Title,A Author,2001,1",
               "3002,1,B Title,B Author,2002,1"
           }),
           "saveToFile writes sorted records with copyId and persists availability");

    LibraryManagementSystem loadedLibrary;
    loadedLibrary.loadFromFile("unit_roundtrip.dat");
    std::vector<Book> loadedBooks = collectBooks(loadedLibrary);

    expectEqual(loadedBooks.size(), static_cast<std::size_t>(3),
                "loadFromFile restores saved records");
    expectEqual(loadedBooks[0].getCopyId(), 1,
                "loadFromFile preserves copy IDs from the saved file");
    expectEqual(loadedBooks[1].getCopyId(), 2,
                "loadFromFile preserves copy IDs for duplicate ISBNs");
    expect(!loadedBooks[0].getAvailability(),
           "loadFromFile restores checked-out availability");
    expect(loadedBooks[1].getAvailability(),
           "loadFromFile restores available copies");
}

void testLibraryLoadClearsExistingAndSkipsIncompleteRows() {
    removeTestDataFiles();

    LibraryManagementSystem library;
    captureStdout([&library]() {
        library.addBook(4001, "Old", "Old Author", 1900);
    });
    expectEqual(collectBooks(library).size(), static_cast<std::size_t>(1),
                "setup record was inserted before load");

    std::ofstream input("unit_load_input.dat");
    input << "\n";
    input << "5002,Valid Two,Author Two,2002,0\n";
    input << "5001,Valid One,Author One,2001,1\n";
    input << "5003,\"Comma, Title\",\"Author \"\"Quoted\"\"\",2003,1\n";
    input << "6001,Incomplete,Author Only\n";
    input << "bad,Invalid ISBN,Author,2004,1\n";
    input << "5004,\"Unclosed title,Author,2004,1\n";
    input.close();

    library.loadFromFile("unit_load_input.dat");
    std::vector<Book> books = collectBooks(library);

    expectEqual(books.size(), static_cast<std::size_t>(3),
                "loadFromFile clears existing catalog and skips incomplete rows");
    expectEqual(books[0].getISBN(), 5001, "loaded books are sorted by ISBN");
    expectEqual(books[1].getISBN(), 5002, "all complete rows are loaded");
    expect(!books[1].getAvailability(), "availability value 0 loads as checked out");
    expectEqual(books[2].getTitle(), std::string("Comma, Title"),
                "loadFromFile parses quoted CSV title fields");
    expectEqual(books[2].getAuthor(), std::string("Author \"Quoted\""),
                "loadFromFile parses escaped quotes in CSV fields");
}

void testLibraryJournalRecoveryWithoutFullSave() {
    removeTestDataFiles();

    LibraryManagementSystem* activeLibrary = new LibraryManagementSystem();
    captureStdout([activeLibrary]() {
        activeLibrary->addBook(7001, "Journaled", "Writer", 2024);
        activeLibrary->checkoutBook(7001);
    });

    expect(fileExists("library.dat.journal"),
           "successful changes append to the journal before full compaction");

    LibraryManagementSystem recoveredLibrary;
    std::vector<Book> recoveredBooks = collectBooks(recoveredLibrary);

    expectEqual(recoveredBooks.size(), static_cast<std::size_t>(1),
                "constructor replays journal entries after loading the snapshot");
    expectEqual(recoveredBooks[0].getISBN(), 7001,
                "journal replay restores the added book");
    expect(!recoveredBooks[0].getAvailability(),
           "journal replay restores checkout state");

    delete activeLibrary;
}

void testLibraryDestructorCompactsJournal() {
    removeTestDataFiles();

    {
        LibraryManagementSystem library;
        captureStdout([&library]() {
            library.addBook(8001, "Compact", "Writer", 2025);
        });
        expect(fileExists("library.dat.journal"),
               "journal exists while changes are pending compaction");
    }

    expect(fileExists("library.dat"),
           "destructor writes pending changes to the snapshot");
    expect(!fileExists("library.dat.journal"),
           "destructor removes the journal after compaction");

    LibraryManagementSystem loadedLibrary;
    std::vector<Book> loadedBooks = collectBooks(loadedLibrary);
    expectEqual(loadedBooks.size(), static_cast<std::size_t>(1),
                "compacted snapshot reloads without journal replay");
    expectEqual(loadedBooks[0].getISBN(), 8001,
                "compacted snapshot contains the saved book");
}

void testThresholdCompactionIncludesTriggeringMutation() {
    removeTestDataFiles();

    {
        LibraryManagementSystem library;
        captureStdout([&library]() {
            for (int i = 0; i < 1000; ++i) {
                library.addBook(900000 + i, "Threshold", "Writer", 2026);
            }
        });

        expectEqual(collectBooks(library).size(), static_cast<std::size_t>(1000),
                    "library keeps all books in memory after threshold compaction");
        expect(fileExists("library.dat"),
               "threshold compaction writes a snapshot immediately");
        expect(!fileExists("library.dat.journal"),
               "threshold compaction removes the fully applied journal");
    }

    LibraryManagementSystem loadedLibrary;
    std::vector<Book> loadedBooks = collectBooks(loadedLibrary);
    expectEqual(loadedBooks.size(), static_cast<std::size_t>(1000),
                "threshold snapshot includes the mutation that triggered compaction");
    expectEqual(loadedBooks.back().getISBN(), 900999,
                "the 1000th added book survives reload after threshold compaction");
}

} // namespace

int main() {
    removeTestDataFiles();

    runCase("Book defaults, display, and serialization", testBookDefaultsAndSerialization);
    runCase("Book ordering and identity", testBookOrderingAndIdentity);
    runCase("RedBlackTree empty, duplicate, and clear behavior", testRedBlackTreeEmptyDuplicateAndClear);
    runCase("RedBlackTree ordering, search, and removal", testRedBlackTreeOrderingSearchAndRemoval);
    runCase("Library empty startup and empty catalog display", testLibraryStartsEmptyAndReportsEmptyCatalog);
    runCase("Library duplicate-copy workflow and user messages", testLibraryCopyWorkflowAndMessages);
    runCase("Library display and persistence", testLibraryDisplayAndPersistence);
    runCase("Library load clears existing data and skips incomplete rows",
            testLibraryLoadClearsExistingAndSkipsIncompleteRows);
    runCase("Library journal recovery without full save",
            testLibraryJournalRecoveryWithoutFullSave);
    runCase("Library destructor compacts journal",
            testLibraryDestructorCompactsJournal);
    runCase("Threshold compaction includes triggering mutation",
            testThresholdCompactionIncludesTriggeringMutation);

    removeTestDataFiles();

    if (failures != 0) {
        std::cerr << failedCases.size() << " test case(s), " << failures
                  << " assertion(s) failed out of " << assertions << "." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << assertions << " assertions passed across 11 test cases." << std::endl;
    return EXIT_SUCCESS;
}
