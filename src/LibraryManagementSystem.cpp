#include "LibraryManagementSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <vector>

namespace {
const std::string DataFilename = "library.dat";
const std::string JournalFilename = "library.dat.journal";

std::vector<std::string> parseCsvFields(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];

        if (inQuotes) {
            if (ch == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    field += '"';
                    ++i;
                } else {
                    inQuotes = false;
                }
            } else {
                field += ch;
            }
        } else if (ch == ',') {
            fields.push_back(field);
            field.clear();
        } else if (ch == '"' && field.empty()) {
            inQuotes = true;
        } else {
            field += ch;
        }
    }

    if (inQuotes) {
        return {};
    }

    fields.push_back(field);
    return fields;
}

bool parseIntField(const std::string& value, int& result) {
    try {
        std::size_t parsed = 0;
        result = std::stoi(value, &parsed);
        return parsed == value.size();
    } catch (...) {
        return false;
    }
}

}

LibraryManagementSystem::LibraryManagementSystem() : dirty(false) {
    loadFromFile(DataFilename);
    replayJournal(JournalFilename);
    /*if (bookTree.isEmpty()) {
        // Add sample books
        addBook(1001, "The Great Gatsby", "F. Scott Fitzgerald", 1925);
        addBook(1002, "To Kill a Mockingbird", "Harper Lee", 1960);
        addBook(1003, "1984", "George Orwell", 1949);
        addBook(1004, "Pride and Prejudice", "Jane Austen", 1813);
        addBook(1005, "The Catcher in the Rye", "J.D. Salinger", 1951);
    }*/
}

LibraryManagementSystem::~LibraryManagementSystem() {
    if (journalStream.is_open()) {
        journalStream.close();
    }
    compactSave();
}

int LibraryManagementSystem::getNextCopyId(int isbn) const {
    auto it = nextCopyIdByIsbn.find(isbn);
    if (it != nextCopyIdByIsbn.end()) {
        return it->second + 1;
    }

    int maxCopyId = 0;
    const Book searchKey(isbn);
    RBNode<Book>* node = bookTree.lowerBound(searchKey);
    while (node != nullptr && node->data.getISBN() == isbn) {
        if (node->data.getCopyId() > maxCopyId) {
            maxCopyId = node->data.getCopyId();
        }
        node = bookTree.successor(node);
    }
    return maxCopyId + 1;
}

RBNode<Book>* LibraryManagementSystem::findBookNode(int isbn) {
    const Book searchKey(isbn);
    RBNode<Book>* node = bookTree.lowerBound(searchKey);
    if (node == nullptr || node->data.getISBN() != isbn) {
        return nullptr;
    }
    return node;
}

RBNode<Book>* LibraryManagementSystem::findAvailableBookNode(int isbn) {
    const Book searchKey(isbn);
    RBNode<Book>* node = bookTree.lowerBound(searchKey);
    while (node != nullptr && node->data.getISBN() == isbn) {
        if (node->data.getAvailability()) {
            return node;
        }
        node = bookTree.successor(node);
    }
    return nullptr;
}

LibraryManagementSystem::IsbnLookup
LibraryManagementSystem::lookupIsbn(int isbn, bool wantAvailable) {
    IsbnLookup result;
    const Book searchKey(isbn);
    RBNode<Book>* node = bookTree.lowerBound(searchKey);

    while (node != nullptr && node->data.getISBN() == isbn) {
        result.isbnExists = true;
        if (node->data.getAvailability() == wantAvailable) {
            result.matchingCopy = node;
            return result;
        }
        node = bookTree.successor(node);
    }
    return result;
}

bool LibraryManagementSystem::recordChange(const std::string& entry) {
    if (!journalStream.is_open()) {
        journalStream.open(JournalFilename, std::ios::app | std::ios::out);
        if (!journalStream.is_open()) {
            std::cerr << "Error: Could not open journal for writing: "
                      << JournalFilename << std::endl;
            return false;
        }
    }

    journalStream << entry << '\n';
    journalStream.flush();

    if (!journalStream) {
        std::cerr << "Error: Could not write journal entry." << std::endl;
        journalStream.close();
        journalStream.clear();
        return false;
    }

    dirty = true;
    ++journalOpsSinceCompact;
    return true;
}

void LibraryManagementSystem::replayJournal(const std::string& filename) {
    std::ifstream journal(filename);
    if (!journal.is_open()) return;

    std::string line;
    bool foundJournalEntries = false;
    while (std::getline(journal, line)) {
        if (line.empty()) continue;
        foundJournalEntries = true;

        const std::vector<std::string> fields = parseCsvFields(line);
        if (fields.empty()) continue;

        const std::string& operation = fields[0];

        if (operation == "ADD") {
            if (fields.size() < 5) continue;

            int isbn = 0;
            int year = 0;
            int copyId = 0;

            if (fields.size() >= 6) {
                // New format: ADD,isbn,copyId,title,author,year
                if (!parseIntField(fields[1], isbn) || !parseIntField(fields[2], copyId)
                    || !parseIntField(fields[5], year)) {
                    continue;
                }
                Book book(isbn, fields[3], fields[4], year, true, copyId);
                bookTree.insert(book);
                nextCopyIdByIsbn[isbn] = std::max(nextCopyIdByIsbn[isbn], copyId);
            } else {
                // Old format (backward compat): ADD,isbn,title,author,year
                if (!parseIntField(fields[1], isbn) || !parseIntField(fields[4], year)) {
                    continue;
                }
                copyId = getNextCopyId(isbn);
                Book book(isbn, fields[2], fields[3], year, true, copyId);
                bookTree.insert(book);
                nextCopyIdByIsbn[isbn] = std::max(nextCopyIdByIsbn[isbn], copyId);
            }
        } else if (operation == "REMOVE") {
            if (fields.size() < 2) continue;

            int isbn = 0;
            if (!parseIntField(fields[1], isbn)) continue;

            if (fields.size() >= 3) {
                // New format: REMOVE,isbn,copyId - target exact copy
                int copyId = 0;
                if (!parseIntField(fields[2], copyId)) continue;
                RBNode<Book>* node = bookTree.search(Book(isbn, "", "", 0, true, copyId));
                if (node != nullptr) {
                    bookTree.remove(node->data);
                }
            } else {
                // Old format: REMOVE,isbn - target first match
                RBNode<Book>* node = findBookNode(isbn);
                if (node != nullptr) {
                    bookTree.remove(node->data);
                }
            }
        } else if (operation == "CHECKOUT") {
            if (fields.size() < 2) continue;

            int isbn = 0;
            if (!parseIntField(fields[1], isbn)) continue;

            if (fields.size() >= 3) {
                // New format: CHECKOUT,isbn,copyId - target exact copy
                int copyId = 0;
                if (!parseIntField(fields[2], copyId)) continue;
                RBNode<Book>* node = bookTree.search(Book(isbn, "", "", 0, true, copyId));
                if (node != nullptr) {
                    node->data.setAvailability(false);
                }
            } else {
                // Old format: CHECKOUT,isbn - target first available
                RBNode<Book>* node = findAvailableBookNode(isbn);
                if (node != nullptr) {
                    node->data.setAvailability(false);
                }
            }
        } else if (operation == "RETURN") {
            if (fields.size() < 2) continue;

            int isbn = 0;
            if (!parseIntField(fields[1], isbn)) continue;

            if (fields.size() >= 3) {
                // New format: RETURN,isbn,copyId - target exact copy
                int copyId = 0;
                if (!parseIntField(fields[2], copyId)) continue;
                RBNode<Book>* node = bookTree.search(Book(isbn, "", "", 0, true, copyId));
                if (node != nullptr) {
                    node->data.setAvailability(true);
                }
            } else {
                // Old format: RETURN,isbn - target first checked-out
                IsbnLookup lookup = lookupIsbn(isbn, false);
                RBNode<Book>* node = lookup.matchingCopy;
                if (node != nullptr) {
                    node->data.setAvailability(true);
                }
            }
        }
    }

    if (foundJournalEntries) {
        dirty = true;
    }
}

void LibraryManagementSystem::compactSave() {
    if (journalStream.is_open()) {
        journalStream.close();
    }
    if (dirty) {
        saveToFile(DataFilename);
        journalOpsSinceCompact = 0;
    }
}

void LibraryManagementSystem::compactIfThresholdReached() {
    if (journalOpsSinceCompact >= COMPACT_THRESHOLD) {
        compactSave();
    }
}

static std::string sanitizeField(const std::string& value) {
    std::string out;
    out.reserve(value.size());
    for (char ch : value) {
        if (ch == '\n' || ch == '\r') {
            out += ' ';
        } else {
            out += ch;
        }
    }
    return out;
}

bool LibraryManagementSystem::addBook(int isbn, const std::string& title,
                                      const std::string& author, int year) {
    int copyId = getNextCopyId(isbn);
    Book newBook(isbn, title, author, year, true, copyId);

    std::string safeTitle = sanitizeField(title);
    std::string safeAuthor = sanitizeField(author);

    std::ostringstream journalEntry;
    journalEntry << "ADD," << isbn << ',' << copyId << ',';
    writeCsvField(journalEntry, safeTitle);
    journalEntry << ',';
    writeCsvField(journalEntry, safeAuthor);
    journalEntry << ',' << year;
    if (!recordChange(journalEntry.str())) {
        return false;
    }

    bookTree.insert(newBook);
    nextCopyIdByIsbn[isbn] = copyId;
    compactIfThresholdReached();
    std::cout << "Book added successfully: " << title
              << " (copy " << copyId << ")" << std::endl;
    return true;
}

bool LibraryManagementSystem::removeBook(int isbn) {
    RBNode<Book>* node = findBookNode(isbn);

    if (node == nullptr) {
        std::cout << "Book with ISBN " << isbn << " not found." << std::endl;
        return false;
    }

    Book targetBook = node->data;
    if (!recordChange("REMOVE," + std::to_string(isbn) + ',' + std::to_string(targetBook.getCopyId()))) {
        return false;
    }

    bookTree.remove(targetBook);
    compactIfThresholdReached();
    std::cout << "Book with ISBN " << isbn << " removed successfully"
              << " (copy " << targetBook.getCopyId() << ")." << std::endl;
    return true;
}

bool LibraryManagementSystem::checkoutBook(int isbn) {
    IsbnLookup lookup = lookupIsbn(isbn, true);

    if (lookup.matchingCopy == nullptr) {
        if (!lookup.isbnExists) {
            std::cout << "Book with ISBN " << isbn << " not found." << std::endl;
        } else {
            std::cout << "All copies of ISBN " << isbn << " are already checked out." << std::endl;
        }
        return false;
    }

    RBNode<Book>* node = lookup.matchingCopy;
    if (!recordChange("CHECKOUT," + std::to_string(isbn) + ',' + std::to_string(node->data.getCopyId()))) {
        return false;
    }

    node->data.setAvailability(false);
    compactIfThresholdReached();
    std::cout << "Book checked out successfully: " << node->data.getTitle()
              << " (copy " << node->data.getCopyId() << ")" << std::endl;
    return true;
}

bool LibraryManagementSystem::returnBook(int isbn) {
    IsbnLookup lookup = lookupIsbn(isbn, false);

    if (lookup.matchingCopy == nullptr) {
        if (!lookup.isbnExists) {
            std::cout << "Book with ISBN " << isbn << " not found." << std::endl;
        } else {
            std::cout << "All copies of ISBN " << isbn << " are already available in the library." << std::endl;
        }
        return false;
    }

    RBNode<Book>* node = lookup.matchingCopy;
    if (!recordChange("RETURN," + std::to_string(isbn) + ',' + std::to_string(node->data.getCopyId()))) {
        return false;
    }

    node->data.setAvailability(true);
    compactIfThresholdReached();
    std::cout << "Book returned successfully: " << node->data.getTitle()
              << " (copy " << node->data.getCopyId() << ")" << std::endl;
    return true;
}

void LibraryManagementSystem::displayBook(int isbn) {
    Book* book = findBook(isbn);

    if (book == nullptr) {
        std::cout << "Book with ISBN " << isbn << " not found." << std::endl;
        return;
    }

    book->display();
}

void LibraryManagementSystem::displayAllBooks() {
    if (bookTree.isEmpty()) {
        std::cout << "The library is empty." << std::endl;
        return;
    }

    std::cout << "\n========== Library Catalog ==========\n";
    bookTree.inorderTraversal();
    std::cout << "====================================\n" << std::endl;
}

void LibraryManagementSystem::loadFromFile(const std::string& filename) {
    if (journalStream.is_open()) {
        journalStream.close();
    }
    if (dirty) {
        compactSave();
    }

    std::ifstream file(filename);
    bookTree.clear();
    nextCopyIdByIsbn.clear();
    journalOpsSinceCompact = 0;
    dirty = false;

    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        const std::vector<std::string> fields = parseCsvFields(line);
        if (fields.size() < 5) continue;

        int isbn = 0;
        int year = 0;
        int copyId = 0;

        if (fields.size() >= 6) {
            // New format: isbn,copyId,title,author,year,available
            if (!parseIntField(fields[0], isbn) || !parseIntField(fields[1], copyId)
                || !parseIntField(fields[4], year)) {
                continue;
            }
            const bool available = (fields[5] == "1");
            Book book(isbn, fields[2], fields[3], year, available, copyId);
            bookTree.insert(book);
            nextCopyIdByIsbn[isbn] = std::max(nextCopyIdByIsbn[isbn], copyId);
        } else {
            // Old format (backward compat): isbn,title,author,year,available
            if (!parseIntField(fields[0], isbn) || !parseIntField(fields[3], year)) {
                continue;
            }
            const bool available = (fields[4] == "1");
            copyId = getNextCopyId(isbn);
            Book book(isbn, fields[1], fields[2], year, available, copyId);
            bookTree.insert(book);
            nextCopyIdByIsbn[isbn] = std::max(nextCopyIdByIsbn[isbn], copyId);
        }
    }
    file.close();
    dirty = false;
}

void LibraryManagementSystem::saveToFile(const std::string& filename) const {
    const std::string tempFilename = filename + ".tmp";
    std::ofstream file(tempFilename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << tempFilename << std::endl;
        return;
    }
    bookTree.inorderTraversal([&file](const Book& book) {
        book.serialize(file);
        file << '\n';
    });
    file.flush();
    file.close();
    if (!file) {
        std::cerr << "Error: Could not finish writing file: " << filename << std::endl;
        std::remove(tempFilename.c_str());
        return;
    }

    if (std::rename(tempFilename.c_str(), filename.c_str()) != 0) {
        std::remove(filename.c_str());
        if (std::rename(tempFilename.c_str(), filename.c_str()) != 0) {
            std::cerr << "Error: Could not replace file: " << filename << std::endl;
            std::remove(tempFilename.c_str());
            return;
        }
    }

    if (filename == DataFilename) {
        std::remove(JournalFilename.c_str());
        dirty = false;
    }
}

void LibraryManagementSystem::forEachBook(std::function<void(const Book&)> func) const {
    bookTree.inorderTraversal(func);
}

Book* LibraryManagementSystem::findBook(int isbn) {
    RBNode<Book>* node = findBookNode(isbn);
    return (node != nullptr) ? &(node->data) : nullptr;
}
