#include "LibraryManagementSystem.h"
#include <iostream>

LibraryManagementSystem::LibraryManagementSystem() {}

void LibraryManagementSystem::addBook(int isbn, const std::string& title, 
                                      const std::string& author, int year) {
    Book newBook(isbn, title, author, year);
    
    // Check if book already exists
    if (findBook(isbn) != nullptr) {
        std::cout << "Book with ISBN " << isbn << " already exists in the library." << std::endl;
        return;
    }
    
    bookTree.insert(newBook);
    std::cout << "Book added successfully: " << title << std::endl;
}

void LibraryManagementSystem::removeBook(int isbn) {
    Book* book = findBook(isbn);
    
    if (book == nullptr) {
        std::cout << "Book with ISBN " << isbn << " not found." << std::endl;
        return;
    }
    
    Book tempBook(isbn, "", "", 0);
    bookTree.remove(tempBook);
    std::cout << "Book with ISBN " << isbn << " removed successfully." << std::endl;
}

void LibraryManagementSystem::checkoutBook(int isbn) {
    RBNode<Book>* node = bookTree.search(Book(isbn, "", "", 0));
    
    if (node == nullptr) {
        std::cout << "Book with ISBN " << isbn << " not found." << std::endl;
        return;
    }
    
    if (!node->data.getAvailability()) {
        std::cout << "Book is already checked out." << std::endl;
        return;
    }
    
    node->data.setAvailability(false);
    std::cout << "Book checked out successfully: " << node->data.getTitle() << std::endl;
}

void LibraryManagementSystem::returnBook(int isbn) {
    RBNode<Book>* node = bookTree.search(Book(isbn, "", "", 0));
    
    if (node == nullptr) {
        std::cout << "Book with ISBN " << isbn << " not found." << std::endl;
        return;
    }
    
    if (node->data.getAvailability()) {
        std::cout << "Book is already available in the library." << std::endl;
        return;
    }
    
    node->data.setAvailability(true);
    std::cout << "Book returned successfully: " << node->data.getTitle() << std::endl;
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
    
    std::cout << "\n========== Library Catalog ==========" << std::endl;
    bookTree.inorderTraversal();
    std::cout << "====================================\n" << std::endl;
}

Book* LibraryManagementSystem::findBook(int isbn) {
    RBNode<Book>* node = bookTree.search(Book(isbn, "", "", 0));
    return (node != nullptr) ? &(node->data) : nullptr;
}
