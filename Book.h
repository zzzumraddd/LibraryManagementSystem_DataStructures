#ifndef BOOK_H
#define BOOK_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

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
    cout << setfill('0')
         << setw(2) << d.day << "/"
         << setw(2) << d.month << "/"
         << setw(4) << d.year << setfill(' ');
}

inline void inputDate(Date &d, const string &prompt) {
    cout << prompt;
    cin >> d.day >> d.month >> d.year;
    while (cin.fail() || d.day <= 0 || d.month <= 0 || d.month > 12 || d.year <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid date, try again (dd mm yyyy): ";
        cin >> d.day >> d.month >> d.year;
    }
}

// ========================= WAITING QUEUE =========================

struct WaitNode {
    string studentId;
    WaitNode *next;
    WaitNode(const string &id) : studentId(id), next(NULL) {}
};

// ========================= ISSUED RECORD =========================

struct IssuedRecord {
    string studentId;
    Date issueDate;
    Date dueDate;
    IssuedRecord *next;

    IssuedRecord(const string &sid, const Date &iss, const Date &due)
            : studentId(sid), issueDate(iss), dueDate(due), next(NULL) {}
};

// ========================= BOOK NODE (LINKED LIST) =========================

struct BookNode {
    int id;
    string title;
    string author;
    int totalCopies;
    int availableCopies;

    WaitNode *waitFront;
    WaitNode *waitRear;

    IssuedRecord *issuedHead;

    BookNode *next;

    BookNode(int _id, const string &_title, const string &_author,
             int total, int avail)
            : id(_id), title(_title), author(_author),
              totalCopies(total), availableCopies(avail),
              waitFront(NULL), waitRear(NULL),
              issuedHead(NULL), next(NULL) {}

    bool isStudentInQueue(const string &studentId) const {
        WaitNode *cur = waitFront;
        while (cur != NULL) {
            if (cur->studentId == studentId) return true;
            cur = cur->next;
        }
        return false;
    }

    void enqueueWait(const string &studentId) {
        WaitNode *node = new WaitNode(studentId);
        if (waitRear == NULL) {
            waitFront = waitRear = node;
        } else {
            waitRear->next = node;
            waitRear = node;
        }
    }

    bool dequeueWait(string &studentIdOut) {
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

    IssuedRecord* findIssued(const string &studentId) {
        IssuedRecord *cur = issuedHead;
        while (cur != NULL) {
            if (cur->studentId == studentId) return cur;
            cur = cur->next;
        }
        return NULL;
    }

    bool removeIssued(const string &studentId, IssuedRecord* &removed) {
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

    void addIssued(const string &studentId, const Date &issueD, const Date &dueD) {
        IssuedRecord *rec = new IssuedRecord(studentId, issueD, dueD);
        rec->next = issuedHead;
        issuedHead = rec;
    }

    void printBrief() const {
        cout << "ID: " << id
             << " | Title: " << title
             << " | Author: " << author
             << " | Total: " << totalCopies
             << " | Available: " << availableCopies;
        int wc = waitingCount();
        if (wc > 0) {
            cout << " | Waiting: " << wc;
        }
        cout << "\n";
    }

    string toFileLine() const {
        ostringstream oss;
        oss << id << "|" << title << "|" << author
            << "|" << totalCopies << "|" << availableCopies;
        return oss.str();
    }

    static BookNode* fromFileLine(const string &line) {
        stringstream ss(line);
        string part;
        int id, total, avail;
        string title, author;

        getline(ss, part, '|');
        if (part.empty()) return NULL;
        id = atoi(part.c_str());

        getline(ss, title, '|');
        getline(ss, author, '|');

        getline(ss, part, '|');
        total = atoi(part.c_str());

        getline(ss, part, '|');
        avail = atoi(part.c_str());

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
