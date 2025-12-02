#ifndef BOOK_H
#define BOOK_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// ========================= DATE UTILITIES =========================

struct Date {
    int day;
    int month;
    int year;

    Date() : day(1), month(1), year(2000) {}
};

inline bool isLeap(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

inline int daysFromStart(const Date &d) {
    static int monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    long long days = (long long)d.year * 365LL + d.day;

    for (int m = 1; m < d.month; ++m) {
        days += monthDays[m-1];
        if (m == 2 && isLeap(d.year)) days += 1;
    }
    int y = d.year - 1;
    days += y/4 - y/100 + y/400;
    return (int)days;
}

inline int daysBetween(const Date &from, const Date &to) {
    return daysFromStart(to) - daysFromStart(from);
}

inline Date addDays(const Date &start, int add) {
    Date d = start;
    static int monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    d.day += add;
    while (true) {
        int md = monthDays[d.month-1];
        if (d.month == 2 && isLeap(d.year)) md = 29;
        if (d.day <= md) break;
        d.day -= md;
        d.month++;
        if (d.month > 12) {
            d.month = 1;
            d.year++;
        }
    }
    return d;
}

inline void printDate(const Date &d) {
    std::cout << std::setfill('0')
              << std::setw(2) << d.day << "/"
              << std::setw(2) << d.month << "/"
              << std::setw(4) << d.year << std::setfill(' ');
}

inline void inputDate(Date &d, const std::string &prompt) {
    std::cout << prompt;
    std::cin >> d.day >> d.month >> d.year;
    while (std::cin.fail() || d.day <= 0 || d.month <= 0 || d.month > 12 || d.year <= 0) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Invalid date, try again (dd mm yyyy): ";
        std::cin >> d.day >> d.month >> d.year;
    }
}

// ========================= WAITING QUEUE =========================

struct WaitNode {
    std::string studentId;
    WaitNode *next;
    WaitNode(const std::string &id) : studentId(id), next(NULL) {}
};

// ========================= ISSUED RECORD =========================

struct IssuedRecord {
    std::string studentId;
    Date issueDate;
    Date dueDate;
    IssuedRecord *next;

    IssuedRecord(const std::string &sid, const Date &iss, const Date &due)
            : studentId(sid), issueDate(iss), dueDate(due), next(NULL) {}
};

// ========================= BOOK NODE (LINKED LIST) =========================

struct BookNode {
    int id;
    std::string title;
    std::string author;
    int totalCopies;
    int availableCopies;

    WaitNode *waitFront;
    WaitNode *waitRear;

    IssuedRecord *issuedHead;

    BookNode *next;

    BookNode(int _id, const std::string &_title, const std::string &_author,
             int total, int avail)
            : id(_id), title(_title), author(_author),
              totalCopies(total), availableCopies(avail),
              waitFront(NULL), waitRear(NULL),
              issuedHead(NULL), next(NULL) {}

    bool isStudentInQueue(const std::string &studentId) const {
        WaitNode *cur = waitFront;
        while (cur != NULL) {
            if (cur->studentId == studentId) return true;
            cur = cur->next;
        }
        return false;
    }

    void enqueueWait(const std::string &studentId) {
        WaitNode *node = new WaitNode(studentId);
        if (waitRear == NULL) {
            waitFront = waitRear = node;
        } else {
            waitRear->next = node;
            waitRear = node;
        }
    }

    bool dequeueWait(std::string &studentIdOut) {
        if (waitFront == NULL) return false;
        WaitNode *node = waitFront;
        studentIdOut = node->studentId;
        waitFront = waitFront->next;
        if (waitFront == NULL) waitRear = NULL;
        delete node;
        return true;
    }

    int waitingCount() const {
        int c = 0;
        WaitNode *cur = waitFront;
        while (cur != NULL) {
            ++c;
            cur = cur->next;
        }
        return c;
    }

    IssuedRecord* findIssued(const std::string &studentId) {
        IssuedRecord *cur = issuedHead;
        while (cur != NULL) {
            if (cur->studentId == studentId) return cur;
            cur = cur->next;
        }
        return NULL;
    }

    bool removeIssued(const std::string &studentId, IssuedRecord* &removed) {
        removed = NULL;
        IssuedRecord *cur = issuedHead;
        IssuedRecord *prev = NULL;
        while (cur != NULL) {
            if (cur->studentId == studentId) {
                if (prev != NULL) prev->next = cur->next;
                else issuedHead = cur->next;
                removed = cur;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    void addIssued(const std::string &studentId, const Date &issueD, const Date &dueD) {
        IssuedRecord *rec = new IssuedRecord(studentId, issueD, dueD);
        rec->next = issuedHead;
        issuedHead = rec;
    }

    void printBrief() const {
        std::cout << "ID: " << id
                  << " | Title: " << title
                  << " | Author: " << author
                  << " | Total: " << totalCopies
                  << " | Available: " << availableCopies;
        int wc = waitingCount();
        if (wc > 0) {
            std::cout << " | Waiting: " << wc;
        }
        std::cout << "\n";
    }

    std::string toFileLine() const {
        std::ostringstream oss;
        oss << id << "|" << title << "|" << author
            << "|" << totalCopies << "|" << availableCopies;
        return oss.str();
    }

    static BookNode* fromFileLine(const std::string &line) {
        std::stringstream ss(line);
        std::string part;
        int id, total, avail;
        std::string title, author;

        std::getline(ss, part, '|');
        if (part.empty()) return NULL;
        id = std::atoi(part.c_str());

        std::getline(ss, title, '|');
        std::getline(ss, author, '|');

        std::getline(ss, part, '|');
        total = std::atoi(part.c_str());

        std::getline(ss, part, '|');
        avail = std::atoi(part.c_str());

        return new BookNode(id, title, author, total, avail);
    }
};

inline void freeBookList(BookNode *head) {
    while (head != NULL) {
        BookNode *next = head->next;
        while (head->waitFront != NULL) {
            WaitNode *wn = head->waitFront;
            head->waitFront = wn->next;
            delete wn;
        }
        while (head->issuedHead != NULL) {
            IssuedRecord *ir = head->issuedHead;
            head->issuedHead = ir->next;
            delete ir;
        }
        delete head;
        head = next;
    }
}

#endif // BOOK_H
