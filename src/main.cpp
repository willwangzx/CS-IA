#include "LibraryManagementSystem.h"
#include <iostream>

void displayMenu() {
    std::cout << "\n===== Library Management System =====" << std::endl;
    std::cout << "1. Add a book" << std::endl;
    std::cout << "2. Remove a book" << std::endl;
    std::cout << "3. Search for a book" << std::endl;
    std::cout << "4. Check out a book" << std::endl;
    std::cout << "5. Return a book" << std::endl;
    std::cout << "6. Display all books" << std::endl;
    std::cout << "7. Exit" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "Enter your choice: ";
}

int main() {
    LibraryManagementSystem library;
    int choice;
    
    // Pre-populate with some sample books
    std::cout << "Initializing library with sample books...\n" << std::endl;
    library.addBook(1001, "The Great Gatsby", "F. Scott Fitzgerald", 1925);
    library.addBook(1002, "To Kill a Mockingbird", "Harper Lee", 1960);
    library.addBook(1003, "1984", "George Orwell", 1949);
    library.addBook(1004, "Pride and Prejudice", "Jane Austen", 1813);
    library.addBook(1005, "The Catcher in the Rye", "J.D. Salinger", 1951);
    
    while (true) {
        displayMenu();
        std::cin >> choice;
        
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }
        
        switch (choice) {
            case 1: {
                int isbn, year;
                std::string title, author;
                
                std::cout << "Enter ISBN: ";
                std::cin >> isbn;
                std::cin.ignore();
                
                std::cout << "Enter title: ";
                std::getline(std::cin, title);
                
                std::cout << "Enter author: ";
                std::getline(std::cin, author);
                
                std::cout << "Enter year: ";
                std::cin >> year;
                
                library.addBook(isbn, title, author, year);
                break;
            }
            
            case 2: {
                int isbn;
                std::cout << "Enter ISBN of book to remove: ";
                std::cin >> isbn;
                library.removeBook(isbn);
                break;
            }
            
            case 3: {
                int isbn;
                std::cout << "Enter ISBN to search: ";
                std::cin >> isbn;
                library.displayBook(isbn);
                break;
            }
            
            case 4: {
                int isbn;
                std::cout << "Enter ISBN to check out: ";
                std::cin >> isbn;
                library.checkoutBook(isbn);
                break;
            }
            
            case 5: {
                int isbn;
                std::cout << "Enter ISBN to return: ";
                std::cin >> isbn;
                library.returnBook(isbn);
                break;
            }
            
            case 6: {
                library.displayAllBooks();
                break;
            }
            
            case 7: {
                std::cout << "Thank you for using the Library Management System!" << std::endl;
                return 0;
            }
            
            default: {
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
            }
        }
    }
    
    return 0;
}
