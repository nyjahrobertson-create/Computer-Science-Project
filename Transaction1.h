Transaction.h
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <iostream>

class Transaction {
private:
    int patronId;
    std::string bookTitle;
    std::string date;

public:
    Transaction(int pid, std::string bt);
    void displayTransaction() const;
    friend std::ostream& operator<<(std::ostream& os, const Transaction& t);
};

#endif

