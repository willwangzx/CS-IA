#ifndef LIBRARYMANAGEMENTSYSTEM_H
#define LIBRARYMANAGEMENTSYSTEM_H

#include "RedBlackTree.h"
#include "Book.h"

class LibraryManagementSystem {
private:
    RedBlackTree<Book> bookTree;

public:
    LibraryManagementSystem();
    
    // Core operations
    void addBook(int isbn, const std::string& title, const std::string& author, int year);
    void removeBook(int isbn);
    void checkoutBook(int isbn);
    void returnBook(int isbn);
    void displayBook(int isbn);
    void displayAllBooks();
    
    // Search operations
    Book* findBook(int isbn);
};

#endif
