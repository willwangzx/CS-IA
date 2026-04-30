#ifndef LIBRARYMANAGEMENTSYSTEM_H
#define LIBRARYMANAGEMENTSYSTEM_H

#include "RedBlackTree.h"
#include "Book.h"
#include<functional>
#include <string>

class LibraryManagementSystem {
private:
    RedBlackTree<Book> bookTree;
    mutable bool dirty;

    int getNextCopyId(int isbn) const;
    RBNode<Book>* findBookNode(int isbn);
    RBNode<Book>* findAvailableBookNode(int isbn);
    bool recordChange(const std::string& entry);
    void replayJournal(const std::string& filename);
    void compactSave();

public:
    LibraryManagementSystem();
    ~LibraryManagementSystem();
    
    // Core operations
    void addBook(int isbn, const std::string& title, const std::string& author, int year);
    void removeBook(int isbn);
    void checkoutBook(int isbn);
    void returnBook(int isbn);
    void displayBook(int isbn);
    void displayAllBooks();
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    void forEachBook(std::function<void(const Book&)> func) const;

    // Search operations
    Book* findBook(int isbn);
};

#endif
