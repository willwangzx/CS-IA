#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <iostream>

class Book {
private:
    int isbn;
    std::string title;
    std::string author;
    int year;
    bool isAvailable;

public:
    // Constructor
    Book(int isbn = 0, const std::string& title = "", 
         const std::string& author = "", int year = 0, bool available = true);
    
    // Getters
    int getISBN() const { return isbn; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    int getYear() const { return year; }
    bool getAvailability() const { return isAvailable; }
    
    // Setters
    void setAvailability(bool available) { isAvailable = available; }
    
    // Comparison operators (based on ISBN)
    bool operator<(const Book& other) const { return isbn < other.isbn; }
    bool operator>(const Book& other) const { return isbn > other.isbn; }
    bool operator==(const Book& other) const { return isbn == other.isbn; }
    
    // Display
    void display() const;
    void serialize(std::ostream& os) const {
        os << isbn << ',' << title << ',' << author << ',' << year << ',' << (isAvailable ? '1' : '0');
    }
};

#endif
