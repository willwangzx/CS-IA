#ifndef LIBRARYMANAGEMENTSYSTEM_H
#define LIBRARYMANAGEMENTSYSTEM_H

#include "RedBlackTree.h"
#include "Book.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <fstream>

class LibraryManagementSystem {
private:
    RedBlackTree<Book> bookTree;
    mutable bool dirty;
    mutable std::unordered_map<int, int> nextCopyIdByIsbn;
    mutable std::ofstream journalStream;
    mutable int journalOpsSinceCompact = 0;
    static const int COMPACT_THRESHOLD = 1000;

    struct IsbnLookup {
        RBNode<Book>* matchingCopy = nullptr;
        bool isbnExists = false;
    };

    int getNextCopyId(int isbn) const;
    RBNode<Book>* findBookNode(int isbn);
    RBNode<Book>* findAvailableBookNode(int isbn);
    IsbnLookup lookupIsbn(int isbn, bool wantAvailable);
    bool recordChange(const std::string& entry);
    void replayJournal(const std::string& filename);
    void compactSave();
    void compactIfThresholdReached();

public:
    LibraryManagementSystem();
    ~LibraryManagementSystem();
    
    // Core operations
    bool addBook(int isbn, const std::string& title, const std::string& author, int year);
    bool removeBook(int isbn);
    bool checkoutBook(int isbn);
    bool returnBook(int isbn);
    void displayBook(int isbn);
    void displayAllBooks();
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    void forEachBook(std::function<void(const Book&)> func) const;

    // Search operations
    Book* findBook(int isbn);
};

#endif
