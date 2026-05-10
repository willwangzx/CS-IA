#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <iostream>

class Book {
private:
    int isbn;
    int copyId;
    std::string title;
    std::string author;
    int year;
    bool isAvailable;

public:
    // Constructor
    Book(int isbn = 0, const std::string& title = "", 
         const std::string& author = "", int year = 0, bool available = true,
         int copyId = 0);
    
    // Getters
    int getISBN() const { return isbn; }
    int getCopyId() const { return copyId; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    int getYear() const { return year; }
    bool getAvailability() const { return isAvailable; }
    
    // Setters
    void setAvailability(bool available) { isAvailable = available; }
    
    // Comparison operators (based on ISBN, then copy id)
    bool operator<(const Book& other) const {
        return isbn < other.isbn || (isbn == other.isbn && copyId < other.copyId);
    }
    bool operator>(const Book& other) const {
        return isbn > other.isbn || (isbn == other.isbn && copyId > other.copyId);
    }
    bool operator==(const Book& other) const {
        return isbn == other.isbn && copyId == other.copyId;
    }
    
    // Display
    void display() const;
    void serialize(std::ostream& os) const {
        auto writeCsvField = [](std::ostream& out, const std::string& value) {
            if (value.find_first_of(",\"\n\r") == std::string::npos) {
                out << value;
                return;
            }

            out << '"';
            for (char ch : value) {
                if (ch == '"') {
                    out << "\"\"";
                } else {
                    out << ch;
                }
            }
            out << '"';
        };

        os << isbn << ',' << copyId << ',';
        writeCsvField(os, title);
        os << ',';
        writeCsvField(os, author);
        os << ',' << year << ',' << (isAvailable ? '1' : '0');
    }
};

#endif
