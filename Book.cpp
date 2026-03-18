#include "Book.h"

Book::Book(int isbn, const std::string& title, 
           const std::string& author, int year, bool available, int copyId)
    : isbn(isbn), copyId(copyId), title(title), author(author), year(year), isAvailable(available) {}

void Book::display() const {
    std::cout << isbn
              << ",copy " << copyId
              << "," << title 
              << "," << author 
              << "," << year 
              << "," << (isAvailable ? "1" : "0") 
              << std::endl;
}
