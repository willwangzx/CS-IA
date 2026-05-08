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

void writeCsvField(std::ostream& out, const std::string& value) {
    if (value.find_first_of(",\"\n\r") == std::string::npos) {
        out << value;
        return;
    }

    out << '"';
    for (char ch : value) {
        if (ch == '"') {
            out << "\"\"";
        } else {
            out << ch;
        }
    }
    out << '"';
}
}

LibraryManagementSystem::LibraryManagementSystem() : dirty(false) {
    loadFromFile(DataFilename);
    replayJournal(JournalFilename);
    /*if (bookTree.isEmpty()) {
        // 添加示例书籍
        addBook(1001, "The Great Gatsby", "F. Scott Fitzgerald", 1925);
        addBook(1002, "To Kill a Mockingbird", "Harper Lee", 1960);
        addBook(1003, "1984", "George Orwell", 1949);
        addBook(1004, "Pride and Prejudice", "Jane Austen", 1813);
        addBook(1005, "The Catcher in the Rye", "J.D. Salinger", 1951);
    }*/
}

LibraryManagementSystem::~LibraryManagementSystem() {
    compactSave();
}

int LibraryManagementSystem::getNextCopyId(int isbn) const {
    int maxCopyId = 0;
    bookTree.inorderTraversal([isbn, &maxCopyId](const Book& book) {
        if (book.getISBN() == isbn && book.getCopyId() > maxCopyId) {
            maxCopyId = book.getCopyId();
        }
    });
    return maxCopyId + 1;
}

RBNode<Book>* LibraryManagementSystem::findBookNode(int isbn) {
    return bookTree.findFirst([isbn](const Book& book) {
        return book.getISBN() == isbn;
    });
}

RBNode<Book>* LibraryManagementSystem::findAvailableBookNode(int isbn) {
    return bookTree.findFirst([isbn](const Book& book) {
        return book.getISBN() == isbn && book.getAvailability();
    });
}

bool LibraryManagementSystem::recordChange(const std::string& entry) {
    std::ofstream journal(JournalFilename, std::ios::app);
    if (!journal.is_open()) {
        std::cerr << "Error: Could not open journal for writing: "
                  << JournalFilename << std::endl;
        return false;
    }

    journal << entry << std::endl;
    if (!journal) {
        std::cerr << "Error: Could not write journal entry." << std::endl;
        return false;
    }

    dirty = true;
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
            if (!parseIntField(fields[1], isbn) || !parseIntField(fields[4], year)) {
                continue;
            }

            Book book(isbn, fields[2], fields[3], year, true, getNextCopyId(isbn));
            bookTree.insert(book);
        } else if (operation == "REMOVE") {
            if (fields.size() < 2) continue;

            int isbn = 0;
            if (!parseIntField(fields[1], isbn)) continue;

            RBNode<Book>* node = findBookNode(isbn);
            if (node != nullptr) {
                bookTree.remove(node->data);
            }
        } else if (operation == "CHECKOUT") {
            if (fields.size() < 2) continue;

            int isbn = 0;
            if (!parseIntField(fields[1], isbn)) continue;

            RBNode<Book>* node = findAvailableBookNode(isbn);
            if (node != nullptr) {
                node->data.setAvailability(false);
            }
        } else if (operation == "RETURN") {
            if (fields.size() < 2) continue;

            int isbn = 0;
            if (!parseIntField(fields[1], isbn)) continue;

            RBNode<Book>* node = bookTree.findFirst([isbn](const Book& book) {
                return book.getISBN() == isbn && !book.getAvailability();
            });
            if (node != nullptr) {
                node->data.setAvailability(true);
            }
        }
    }

    if (foundJournalEntries) {
        dirty = true;
    }
}

void LibraryManagementSystem::compactSave() {
    if (dirty) {
        saveToFile(DataFilename);
    }
}

bool LibraryManagementSystem::addBook(int isbn, const std::string& title,
                                      const std::string& author, int year) {
    Book newBook(isbn, title, author, year, true, getNextCopyId(isbn));
    std::ostringstream journalEntry;
    journalEntry << "ADD," << isbn << ',';
    writeCsvField(journalEntry, title);
    journalEntry << ',';
    writeCsvField(journalEntry, author);
    journalEntry << ',' << year;
    if (!recordChange(journalEntry.str())) {
        return false;
    }

    bookTree.insert(newBook);
    std::cout << "Book added successfully: " << title
              << " (copy " << newBook.getCopyId() << ")" << std::endl;
    return true;
}

bool LibraryManagementSystem::removeBook(int isbn) {
    RBNode<Book>* node = findBookNode(isbn);

    if (node == nullptr) {
        std::cout << "Book with ISBN " << isbn << " not found." << std::endl;
        return false;
    }

    Book targetBook = node->data;
    if (!recordChange("REMOVE," + std::to_string(isbn))) {
        return false;
    }

    bookTree.remove(targetBook);
    std::cout << "Book with ISBN " << isbn << " removed successfully"
              << " (copy " << targetBook.getCopyId() << ")." << std::endl;
    return true;
}

bool LibraryManagementSystem::checkoutBook(int isbn) {
    RBNode<Book>* node = findAvailableBookNode(isbn);

    if (node == nullptr) {
        RBNode<Book>* existingNode = findBookNode(isbn);
        if (existingNode == nullptr) {
            std::cout << "Book with ISBN " << isbn << " not found." << std::endl;
        } else {
            std::cout << "All copies of ISBN " << isbn << " are already checked out." << std::endl;
        }
        return false;
    }

    if (!recordChange("CHECKOUT," + std::to_string(isbn))) {
        return false;
    }

    node->data.setAvailability(false);
    std::cout << "Book checked out successfully: " << node->data.getTitle()
              << " (copy " << node->data.getCopyId() << ")" << std::endl;
    return true;
}

bool LibraryManagementSystem::returnBook(int isbn) {
    RBNode<Book>* node = bookTree.findFirst([isbn](const Book& book) {
        return book.getISBN() == isbn && !book.getAvailability();
    });

    if (node == nullptr) {
        RBNode<Book>* existingNode = findBookNode(isbn);
        if (existingNode == nullptr) {
            std::cout << "Book with ISBN " << isbn << " not found." << std::endl;
        } else {
            std::cout << "All copies of ISBN " << isbn << " are already available in the library." << std::endl;
        }
        return false;
    }

    if (!recordChange("RETURN," + std::to_string(isbn))) {
        return false;
    }

    node->data.setAvailability(true);
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
    if (dirty) {
        compactSave();
    }

    std::ifstream file(filename);
    bookTree.clear();
    dirty = false;

    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        const std::vector<std::string> fields = parseCsvFields(line);
        if (fields.size() < 5) continue;

        int isbn = 0;
        int year = 0;
        if (!parseIntField(fields[0], isbn) || !parseIntField(fields[3], year)) {
            continue;
        }

        const bool available = (fields[4] == "1");

        Book book(isbn, fields[1], fields[2], year, available, getNextCopyId(isbn));
        bookTree.insert(book);
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
        file << std::endl;
    });
    file.close();
    if (!file) {
        std::cerr << "Error: Could not finish writing file: " << filename << std::endl;
        std::remove(tempFilename.c_str());
        return;
    }

    std::remove(filename.c_str());
    if (std::rename(tempFilename.c_str(), filename.c_str()) != 0) {
        std::cerr << "Error: Could not replace file: " << filename << std::endl;
        std::remove(tempFilename.c_str());
        return;
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
