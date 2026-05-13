patron.cpp
#include "Patron.h"
#include <algorithm>

Patron::Patron(std::string n, int i) : name(n), id(i) {}

void Patron::borrowBook(Book* b) {
    borrowedBooks.push_back(b);
}

void Patron::returnBook(Book* b) {
    auto it = std::remove(borrowedBooks.begin(), borrowedBooks.end(), b);
    if (it != borrowedBooks.end()) {
        borrowedBooks.erase(it, borrowedBooks.end());
    }
}

void Patron::displayPatron() const {
    std::cout << "Patron ID: " << id << "\nName: " << name << "\nBorrowed Books Count: " << borrowedBooks.size() << "\n";
}

int Patron::getId() const { return id; }

bool Patron::operator==(const Patron& other) const {
    return id == other.id;
}

std::ostream& operator<<(std::ostream& os, const Patron& p) {
    os << p.id << "," << p.name;
    return os;
}
