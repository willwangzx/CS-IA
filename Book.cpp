#include "Book.h"

Book::Book(int isbn, const std::string& title, 
           const std::string& author, int year, bool available)
    : isbn(isbn), title(title), author(author), year(year), isAvailable(available) {}

void Book::display() const {
    std::cout << "ISBN: " << isbn 
              << " | Title: " << title 
              << " | Author: " << author 
              << " | Year: " << year 
              << " | Status: " << (isAvailable ? "Available" : "Checked Out") 
              << std::endl;
}
