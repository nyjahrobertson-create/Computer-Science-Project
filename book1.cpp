book.cpp
#include "Book.h"

Book::Book(std::string t, std::string a, Genre g) 
    : title(t), author(a), genre(g), status(BookStatus::Available) {}

void Book::displayInfo() const {
    std::cout << "Title: " << title << "\nAuthor: " << author 
              << "\nStatus: " << (status == BookStatus::Available ? "Available" : "Checked Out");
}

void Book::setStatus(BookStatus s) { status = s; }
BookStatus BookStatus::getStatus() const { return status; }
std::string Book::getTitle() const { return title; }
std::string Book::getAuthor() const { return author; }
Genre Book::getGenre() const { return genre; }

bool Book::operator==(const Book& other) const {
    return title == other.title && author == other.author;
}

std::ostream& operator<<(std::ostream& os, const Book& b) {
    std::string gStr;
    switch(b.genre) {
        case Genre::Fiction: gStr = "Fiction"; break;
        case Genre::NonFiction: gStr = "NonFiction"; break;
        case Genre::Mystery: gStr = "Mystery"; break;
        case Genre::Science: gStr = "Science"; break;
        case Genre::Biography: gStr = "Biography"; break;
    }
    os << gStr << "," << b.title << "," << b.author;
    return os;
}

// EBook Implementations
EBook::EBook(std::string t, std::string a, Genre g, double size) 
    : Book(t, a, g), fileSizeMB(size) {}

void EBook::displayInfo() const {
    Book::displayInfo();
    std::cout << "\nFormat: EBook\nFile Size: " << fileSizeMB << " MB\n";
}

// PrintedBook Implementations
PrintedBook::PrintedBook(std::string t, std::string a, Genre g, int pages) 
    : Book(t, a, g), pageCount(pages) {}

void PrintedBook::displayInfo() const {
    Book::displayInfo();
    std::cout << "\nFormat: Printed\nPage Count: " << pageCount << " pages\n";
}
