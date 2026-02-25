#include "LibraryManagementSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

LibraryManagementSystem::LibraryManagementSystem() {
    loadFromFile("library.dat");
    /*if (bookTree.isEmpty()) {
        // 添加示例书籍
        addBook(1001, "The Great Gatsby", "F. Scott Fitzgerald", 1925);
        addBook(1002, "To Kill a Mockingbird", "Harper Lee", 1960);
        addBook(1003, "1984", "George Orwell", 1949);
        addBook(1004, "Pride and Prejudice", "Jane Austen", 1813);
        addBook(1005, "The Catcher in the Rye", "J.D. Salinger", 1951);
    }*/
}

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
    saveToFile("library.dat");
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
    saveToFile("library.dat");
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
    saveToFile("library.dat");
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
    saveToFile("library.dat");
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

void LibraryManagementSystem::loadFromFile(const std::string& filename) {
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

        Book book(isbn, title, author, year, available);
        bookTree.insert(book);
    }
    file.close();
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
}

void LibraryManagementSystem::forEachBook(std::function<void(const Book&)> func) const {
    bookTree.inorderTraversal(func);
}

Book* LibraryManagementSystem::findBook(int isbn) {
    RBNode<Book>* node = bookTree.search(Book(isbn, "", "", 0));
    return (node != nullptr) ? &(node->data) : nullptr;
}
