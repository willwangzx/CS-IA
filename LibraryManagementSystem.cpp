#include "LibraryManagementSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

namespace {
const std::string DataFilename = "library.dat";
const std::string JournalFilename = "library.dat.journal";
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

        std::stringstream ss(line);
        std::string operation;
        if (!std::getline(ss, operation, ',')) continue;

        if (operation == "ADD") {
            std::string token;
            if (!std::getline(ss, token, ',')) continue;
            int isbn = std::stoi(token);
            if (!std::getline(ss, token, ',')) continue;
            std::string title = token;
            if (!std::getline(ss, token, ',')) continue;
            std::string author = token;
            if (!std::getline(ss, token, ',')) continue;
            int year = std::stoi(token);

            Book book(isbn, title, author, year, true, getNextCopyId(isbn));
            bookTree.insert(book);
        } else if (operation == "REMOVE") {
            std::string token;
            if (!std::getline(ss, token, ',')) continue;
            RBNode<Book>* node = findBookNode(std::stoi(token));
            if (node != nullptr) {
                bookTree.remove(node->data);
            }
        } else if (operation == "CHECKOUT") {
            std::string token;
            if (!std::getline(ss, token, ',')) continue;
            RBNode<Book>* node = findAvailableBookNode(std::stoi(token));
            if (node != nullptr) {
                node->data.setAvailability(false);
            }
        } else if (operation == "RETURN") {
            std::string token;
            if (!std::getline(ss, token, ',')) continue;
            const int isbn = std::stoi(token);
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
        dirty = false;
    }
}

void LibraryManagementSystem::addBook(int isbn, const std::string& title,
                                      const std::string& author, int year) {
    Book newBook(isbn, title, author, year, true, getNextCopyId(isbn));
    std::ostringstream journalEntry;
    journalEntry << "ADD," << isbn << ',' << title << ',' << author << ',' << year;
    if (!recordChange(journalEntry.str())) {
        return;
    }

    bookTree.insert(newBook);
    std::cout << "Book added successfully: " << title
              << " (copy " << newBook.getCopyId() << ")" << std::endl;
}

void LibraryManagementSystem::removeBook(int isbn) {
    RBNode<Book>* node = findBookNode(isbn);

    if (node == nullptr) {
        std::cout << "Book with ISBN " << isbn << " not found." << std::endl;
        return;
    }

    Book targetBook = node->data;
    if (!recordChange("REMOVE," + std::to_string(isbn))) {
        return;
    }

    bookTree.remove(targetBook);
    std::cout << "Book with ISBN " << isbn << " removed successfully"
              << " (copy " << targetBook.getCopyId() << ")." << std::endl;
}

void LibraryManagementSystem::checkoutBook(int isbn) {
    RBNode<Book>* node = findAvailableBookNode(isbn);

    if (node == nullptr) {
        RBNode<Book>* existingNode = findBookNode(isbn);
        if (existingNode == nullptr) {
            std::cout << "Book with ISBN " << isbn << " not found." << std::endl;
        } else {
            std::cout << "All copies of ISBN " << isbn << " are already checked out." << std::endl;
        }
        return;
    }

    if (!recordChange("CHECKOUT," + std::to_string(isbn))) {
        return;
    }

    node->data.setAvailability(false);
    std::cout << "Book checked out successfully: " << node->data.getTitle()
              << " (copy " << node->data.getCopyId() << ")" << std::endl;
}

void LibraryManagementSystem::returnBook(int isbn) {
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
        return;
    }

    if (!recordChange("RETURN," + std::to_string(isbn))) {
        return;
    }

    node->data.setAvailability(true);
    std::cout << "Book returned successfully: " << node->data.getTitle()
              << " (copy " << node->data.getCopyId() << ")" << std::endl;
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
    if (!file.is_open()) return;  // 文件不存在则静默返回

    bookTree.clear();  // 清空当前树
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;

        // 解析 ISBN
        if (!std::getline(ss, token, ',')) continue;
        int isbn = std::stoi(token);

        // 解析标题
        if (!std::getline(ss, token, ',')) continue;
        std::string title = token;

        // 解析作者
        if (!std::getline(ss, token, ',')) continue;
        std::string author = token;

        // 解析年份
        if (!std::getline(ss, token, ',')) continue;
        int year = std::stoi(token);

        // 解析可用性
        if (!std::getline(ss, token, ',')) continue;
        bool available = (token == "1");

        Book book(isbn, title, author, year, available, getNextCopyId(isbn));
        bookTree.insert(book);
    }
    file.close();
    dirty = false;
}

void LibraryManagementSystem::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
    bookTree.inorderTraversal([&file](const Book& book) {
        book.serialize(file);
        file << std::endl;
    });
    file.close();
    if (!file) {
        std::cerr << "Error: Could not finish writing file: " << filename << std::endl;
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
