main.cpp
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <ctime>

// ==========================================
// ENUMS & CONSTRUCTS
// ==========================================
enum class Genre { Fiction, NonFiction, Mystery, Science, Biography };
enum class BookStatus { Available, CheckedOut };

std::string genreToString(Genre g) {
    switch(g) {
        case Genre::Fiction: return "Fiction";
        case Genre::NonFiction: return "NonFiction";
        case Genre::Mystery: return "Mystery";
        case Genre::Science: return "Science";
        case Genre::Biography: return "Biography";
        default: return "Unknown";
    }
}

Genre stringToGenre(const std::string& s) {
    if (s == "Fiction") return Genre::Fiction;
    if (s == "NonFiction") return Genre::NonFiction;
    if (s == "Mystery") return Genre::Mystery;
    if (s == "Science") return Genre::Science;
    return Genre::Biography;
}

// ==========================================
// 1. BASE CLASS: BOOK
// ==========================================
class Book {
protected:
    std::string title;
    std::string author;
    Genre genre;
    BookStatus status;
public:
    Book(std::string t, std::string a, Genre g) 
        : title(t), author(a), genre(g), status(BookStatus::Available) {}
    virtual ~Book() = default;

    virtual void displayInfo() const {
        std::cout << "Title: " << title << " | Author: " << author 
                  << " | Genre: " << genreToString(genre) 
                  << " | Status: " << (status == BookStatus::Available ? "Available" : "Checked Out");
    }

    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    Genre getGenre() const { return genre; }
    void setStatus(BookStatus s) { status = s; }
    BookStatus getStatus() const { return status; }

    bool operator==(const Book& other) const { return this->title == other.title; }
    friend std::ostream& operator<<(std::ostream& os, const Book& b) {
        os << b.title << " by " << b.author;
        return os;
    }
    virtual std::string toFileString() const = 0;
};

// ==========================================
// 2. DERIVED CLASSES: EBOOK & PRINTEDBOOK
// ==========================================
class EBook : public Book {
    double fileSizeMB;
public:
    EBook(std::string t, std::string a, Genre g, double size) 
        : Book(t, a, g), fileSizeMB(size) {}

    void displayInfo() const override {
        Book::displayInfo();
        std::cout << " | Type: EBook (" << fileSizeMB << "MB)\n";
    }

    std::string toFileString() const override {
        return genreToString(genre) + "," + title + "," + author + ",EBook," + std::to_string(fileSizeMB);
    }
};

class PrintedBook : public Book {
    int pageCount;
public:
    PrintedBook(std::string t, std::string a, Genre g, int pages) 
        : Book(t, a, g), pageCount(pages) {}

    void displayInfo() const override {
        Book::displayInfo();
        std::cout << " | Type: Printed (" << pageCount << " pages)\n";
    }

    std::string toFileString() const override {
        return genreToString(genre) + "," + title + "," + author + ",Printed," + std::to_string(pageCount);
    }
};

// ==========================================
// 3. CLASS: PATRON
// ==========================================
class Patron {
    std::string name;
    int id;
    std::vector<Book*> borrowedBooks;
public:
    Patron(std::string n, int i) : name(n), id(i) {}

    void borrowBook(Book* b) { borrowedBooks.push_back(b); }
    void returnBook(Book* b) {
        auto it = std::remove(borrowedBooks.begin(), borrowedBooks.end(), b);
        if (it != borrowedBooks.end()) {
            borrowedBooks.erase(it, borrowedBooks.end());
        }
    }

    int getId() const { return id; }
    std::string getName() const { return name; }
    const std::vector<Book*>& getBorrowedBooks() const { return borrowedBooks; }

    void displayPatron() const {
        std::cout << "Patron ID: " << id << " | Name: " << name 
                  << " | Borrowed Books Count: " << borrowedBooks.size() << "\n";
    }

    bool operator==(const Patron& other) const { return this->id == other.id; }
    friend std::ostream& operator<<(std::ostream& os, const Patron& p) {
        os << p.name << " (ID: " << p.id << ")";
        return os;
    }
};

// ==========================================
// 4. CLASS: TRANSACTION
// ==========================================
class Transaction {
    int patronId;
    std::string bookTitle;
    std::string date;
public:
    Transaction(int pid, std::string bt) : patronId(pid), bookTitle(bt) {
        std::time_t t = std::time(nullptr);
        char mbstr[100];
        if (std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %H:%M:%S", std::localtime(&t))) {
            date = mbstr;
        }
    }
    void displayTransaction() const {
        std::cout << "[" << date << "] Patron ID " << patronId << " processed item: \"" << bookTitle << "\"\n";
    }
    std::string toFileString() const {
        return std::to_string(patronId) + "," + bookTitle + "," + date;
    }
};

// ==========================================
// 5. CLASS: LIBRARY
// ==========================================
class Library {
    std::vector<Book*> books;
    std::vector<Patron> patrons;
    std::vector<Transaction> transactions;

    void createDefaultFilesIfMissing() {
        std::ifstream bFile("books.txt");
        if (!bFile) {
            std::ofstream out("books.txt");
            out << "Fiction,The Great Gatsby,F. Scott Fitzgerald,Printed,180\n";
            out << "Science,Quantum Physics 101,Albert Einstein,EBook,4.5\n";
            out << "Mystery,Sherlock Holmes,Arthur Conan Doyle,Printed,320\n";
        }
        std::ifstream pFile("patrons.txt");
        if (!pFile) {
            std::ofstream out("patrons.txt");
            out << "1,Jane Doe\n";
            out << "2,John Smith\n";
        }
    }

public:
    Library() { createDefaultFilesIfMissing(); }
    ~Library() {
        for (auto b : books) delete b;
    }

    void loadData() {
        // Load Books
        std::ifstream bFile("books.txt");
        if (!bFile) throw std::runtime_error("Failed to open books file.");
        std::string line;
        while (std::getline(bFile, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string genreStr, title, author, type, extra;
            std::getline(ss, genreStr, ',');
            std::getline(ss, title, ',');
            std::getline(ss, author, ',');
            std::getline(ss, type, ',');
            std::getline(ss, extra, ',');
            
            Genre g = stringToGenre(genreStr);
            if (type == "EBook") {
                books.push_back(new EBook(title, author, g, std::stod(extra)));
            } else {
                books.push_back(new PrintedBook(title, author, g, std::stoi(extra)));
            }
        }

        // Load Patrons
        std::ifstream pFile("patrons.txt");
        if (!pFile) throw std::runtime_error("Failed to open patrons file.");
        while (std::getline(pFile, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string idStr, name;
            std::getline(ss, idStr, ',');
            std::getline(ss, name);
            patrons.push_back(Patron(name, std::stoi(idStr)));
        }
    }

    void saveData() {
        std::ofstream bFile("books.txt");
        for (auto b : books) bFile << b->toFileString() << "\n";

        std::ofstream pFile("patrons.txt");
        for (const auto& p : patrons) pFile << p.getId() << "," << p.getName() << "\n";

        std::ofstream tFile("transactions.txt", std::ios::app);
        for (const auto& t : transactions) tFile << t.toFileString() << "\n";
    }

    void addBook(Book* b) { books.push_back(b); }
    void addPatron(const Patron& p) { patrons.push_back(p); }

    void checkoutBook(int patronId, std::string title) {
        auto pIt = std::find_if(patrons.begin(), patrons.end(), [patronId](const Patron& p) { return p.getId() == patronId; });
        if (pIt == patrons.end()) throw std::runtime_error("Patron ID not found.");

        auto bIt = std::find_if(books.begin(), books.end(), [title](Book* b) { return b->getTitle() == title; });
        if (bIt == books.end()) throw std::runtime_error("Book title not found in library.");

        if ((*bIt)->getStatus() == BookStatus::CheckedOut) throw std::runtime_error("Book is already checked out.");

        (*bIt)->setStatus(BookStatus::CheckedOut);
        pIt->borrowBook(*bIt);
        transactions.push_back(Transaction(patronId, title));
        std::cout << "Success: \"" << title << "\" checked out to " << pIt->getName() << ".\n";
    }

    void returnBook(int patronId, std::string title) {
        auto pIt = std::find_if(patrons.begin(), patrons.end(), [patronId](const Patron& p) { return p.getId() == patronId; });
        if (pIt == patrons.end()) throw std::runtime_error("Patron ID not found.");

        auto bIt = std::find_if(books.begin(), books.end(), [title](Book* b) { return b->getTitle() == title; });
        if (bIt == books.end()) throw std::runtime_error("Book title not found.");

        (*bIt)->setStatus(BookStatus::Available);
        pIt->returnBook(*bIt);
        transactions.push_back(Transaction(patronId, "Returned: " + title));
        std::cout << "Success: \"" << title << "\" returned by " << pIt->getName() << ".\n";
    }

    void searchByAuthorOrGenre(std::string query) {
        std::cout << "\n--- Search Results for '" << query << "' ---\n";
        bool found = false;
        for (auto b : books) {
            if (b->getAuthor() == query || genreToString(b->getGenre()) == query) {
                b->displayInfo();
                found = true;
            }
        }
        if (!found) std::cout << "No matching books found.\n";
    }

    void displayBooks() const {
        std::cout << "\n--- Library Inventory ---\n";
        for (auto b : books) b->displayInfo();
    }

    void displayPatrons() const {
        std::cout << "\n--- Registered Patrons ---\n";
        for (const auto& p : patrons) p.displayPatron();
    }
};

// ==========================================
// MAIN INTERACTION MENU
// ==========================================
int main() {
    Library library;
    try {
        library.loadData();
    } catch (const std::exception& e) {
        std::cerr << "Initialization Error: " << e.what() << std::endl;
        return 1;
    }

    int choice = 0;
    while (choice != 6) {
        std::cout << "\n===============================\n";
        std::cout << "  LIBRARY MANAGEMENT INTERFACE \n";
        std::cout << "===============================\n";
        std::cout << "1. Display All Books\n";
        std::cout << "2. Display All Patrons\n";
        std::cout << "3. Check Out a Book\n";
        std::cout << "4. Return a Book\n";
        std::cout << "5. Search Books (Author/Genre)\n";
        std::cout << "6. Save and Exit\n";
        std::cout << "Enter selection (1-6): ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a digit.\n";
            continue;
        }

        std::cin.ignore(); // Clean buffer

        try {
            if (choice == 1) {
                library.displayBooks();
            } 
            else if (choice == 2) {
                library.displayPatrons();
            } 
            else if (choice == 3) {
                int pid;
                std::string title;
                std::cout << "Enter Patron ID: ";
                std::cin >> pid;
                std::cin.ignore();
                std::cout << "Enter Book Title: ";
                std::getline(std::cin, title);
                library.checkoutBook(pid, title);
            } 
            else if (choice == 4) {
                int pid;
                std::string title;
                std::cout << "Enter Patron ID: ";
                std::cin >> pid;
                std::cin.ignore();
                std::cout << "Enter Book Title: ";
                std::getline(std::cin, title);
                library.returnBook(pid, title);
            } 
            else if (choice == 5) {
                std::string query;
                std::cout << "Enter Author Name or Genre (e.g., Fiction, Science): ";
                std::getline(std::cin, query);
                library.searchByAuthorOrGenre(query);
            }
            else if (choice == 6) {
                library.saveData();
                std::cout << "Data saved successfully. Exiting program.\n";
            }
        } 
        catch (const std::exception& e) {
            std::cerr << "\n[Application Error]: " << e.what() << "\n";
        }
    }
    return 0;
}

