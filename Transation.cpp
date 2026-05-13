Transation.cpp
#include "Transaction.h"
#include <chrono>
#include <iomanip>
#include <sstream>

Transaction::Transaction(int pid, std::string bt) : patronId(pid), bookTitle(bt) {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    
    struct tm buf;
#if defined(_MSC_VER)
    localtime_s(&buf, &in_time_t);
#else
    localtime_r(&in_time_t, &buf);
#endif

    ss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
    date = ss.str();
}

void Transaction::displayTransaction() const {
    std::cout << "[" << date << "] Patron ID: " << patronId << " Action: " << bookTitle << "\n";
}

std::ostream& operator<<(std::ostream& os, const Transaction& t) {
    os << t.date << "," << t.patronId << "," << t.bookTitle;
    return os;
}
