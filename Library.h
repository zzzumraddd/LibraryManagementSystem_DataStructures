#ifndef LIBRARY_H
#define LIBRARY_H

#include "Book.h"
#include <fstream>
#include <limits>
using namespace std;

class Library {
    BookNode *head;
    string dbFile;
    int loanDays;
    int finePerDay;

public:
    Library(const string &file = "books.txt")
            : head(NULL), dbFile(file), loanDays(14), finePerDay(1000) {}

    ~Library() {
        freeBookList(head);
    }

    // ---------- FILE I/O ----------
    void loadFromFile() {
        ifstream fin(dbFile.c_str());
        if (!fin) {
            cout << "No existing book database found, starting empty.\n";
            return;
        }
        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            BookNode *node = BookNode::fromFileLine(line);
            if (node) insertSorted(node);
        }
        fin.close();
    }

    void saveToFile() {
        ofstream fout(dbFile.c_str());
        BookNode *cur = head;
        while (cur != NULL) {
            fout << cur->toFileLine() << "\n";
            cur = cur->next;
        }
        fout.close();
    }

    // ---------- BASIC LIST OPS ----------
    BookNode* findById(int id) const {
        BookNode *cur = head;
        while (cur != NULL) {
            if (cur->id == id) return cur;
            cur = cur->next;
        }
        return NULL;
    }

    bool existsId(int id) const {
        return findById(id) != NULL;
    }

    void insertSorted(BookNode *node) {
        if (head == NULL || node->id < head->id) {
            node->next = head;
            head = node;
            return;
        }
        BookNode *cur = head;
        while (cur->next != NULL && cur->next->id < node->id) {
            cur = cur->next;
        }
        node->next = cur->next;
        cur->next = node;
    }

    void deleteBookById(int id) {
        if (head == NULL) return;
        if (head->id == id) {
            BookNode *tmp = head;
            head = head->next;
            tmp->next = NULL;
            freeBookList(tmp);
            cout << "Book deleted.\n";
            return;
        }
        BookNode *cur = head;
        BookNode *prev = NULL;
        while (cur != NULL && cur->id != id) {
            prev = cur;
            cur = cur->next;
        }
        if (cur == NULL) {
            cout << "Book not found.\n";
            return;
        }
        prev->next = cur->next;
        cur->next = NULL;
        freeBookList(cur);
        cout << "Book deleted.\n";
    }

    // ---------- CORE FEATURES ----------
    void addBookInteractive() {
        int id, total;
        string title, author;

        cout << "Enter Book ID (integer): ";
        cin >> id;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid ID.\n";
            return;
        }
        if (existsId(id)) {
            cout << "Book with this ID already exists.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter title: ";
        getline(cin, title);
        cout << "Enter author: ";
        getline(cin, author);

        cout << "Total copies: ";
        cin >> total;
        if (total <= 0) {
            cout << "Total copies must be positive.\n";
            return;
        }
        int available = total;

        BookNode *node = new BookNode(id, title, author, total, available);
        insertSorted(node);
        cout << "Book added successfully.\n";
    }

    void searchMenu() const {
        if (head == NULL) {
            cout << "No books in library.\n";
            return;
        }
        int choice;
        cout << "\nSearch by:\n";
        cout << "1. Book ID\n";
        cout << "2. Title\n";
        cout << "3. Author\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            int id;
            cout << "Enter ID: ";
            cin >> id;
            BookNode *b = findById(id);
            if (b == NULL) cout << "Book not found.\n";
            else printBookDetails(b);
        } else if (choice == 2) {
            string q;
            cout << "Enter title keyword: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, q);
            searchByTitle(q);
        } else if (choice == 3) {
            string q;
            cout << "Enter author keyword: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, q);
            searchByAuthor(q);
        } else {
            cout << "Invalid choice.\n";
        }
    }

    void searchByTitle(const string &q) const {
        BookNode *cur = head;
        bool found = false;
        while (cur != NULL) {
            if (cur->title.find(q) != string::npos) {
                printBookDetails(cur);
                found = true;
            }
            cur = cur->next;
        }
        if (!found) cout << "No books with given title.\n";
    }

    void searchByAuthor(const string &q) const {
        BookNode *cur = head;
        bool found = false;
        while (cur != NULL) {
            if (cur->author.find(q) != string::npos) {
                printBookDetails(cur);
                found = true;
            }
            cur = cur->next;
        }
        if (!found) cout << "No books with given author.\n";
    }

    void printBookDetails(BookNode *b) const {
        cout << "-----------------------------\n";
        cout << "Book ID: " << b->id << "\n";
        cout << "Title: " << b->title << "\n";
        cout << "Author: " << b->author << "\n";
        cout << "Total copies: " << b->totalCopies << "\n";
        cout << "Available copies: " << b->availableCopies << "\n";
        cout << "In waiting queue: " << b->waitingCount() << "\n";
        int cnt = 0;
        IssuedRecord *ir = b->issuedHead;
        while (ir != NULL) {
            ++cnt;
            ir = ir->next;
        }
        cout << "Currently issued: " << cnt << "\n";
        cout << "-----------------------------\n";
    }

    void displayAll() const {
        if (head == NULL) {
            cout << "No books in library.\n";
            return;
        }
        cout << "\n======= All Books =======\n";
        BookNode *cur = head;
        while (cur != NULL) {
            cur->printBrief();
            cur = cur->next;
        }
        cout << "=========================\n";
    }

    void issueBook(const string &studentId) {
        int id;
        cout << "Enter Book ID to issue: ";
        cin >> id;
        BookNode *b = findById(id);
        if (b == NULL) {
            cout << "Book not found.\n";
            return;
        }

        if (b->findIssued(studentId) != NULL) {
            cout << "You already have this book issued.\n";
            return;
        }

        if (b->availableCopies > 0) {
            Date issueDate;
            inputDate(issueDate, "Enter issue date (dd mm yyyy): ");
            Date due = addDays(issueDate, loanDays);
            b->addIssued(studentId, issueDate, due);
            b->availableCopies--;

            cout << "Book issued successfully.\n";
            cout << "Due date: ";
            printDate(due);
            cout << "\n";
        } else {
            if (b->isStudentInQueue(studentId)) {
                cout << "You are already in the waiting queue.\n";
                return;
            }
            b->enqueueWait(studentId);
            int pos = b->waitingCount();
            cout << "No copies available now. You are added to waiting list.\n";
            cout << "Your position in queue: " << pos << "\n";
        }
    }

    void returnBook(const string &studentIdOpt = "") {
        int id;
        cout << "Enter Book ID to return: ";
        cin >> id;
        BookNode *b = findById(id);
        if (b == NULL) {
            cout << "Book not found.\n";
            return;
        }

        string studentId = studentIdOpt;
        if (studentId.empty()) {
            cout << "Enter Student ID who is returning: ";
            cin >> studentId;
        }

        IssuedRecord *rec = NULL;
        if (!b->removeIssued(studentId, rec)) {
            cout << "This student does not have this book.\n";
            return;
        }

        Date returnDate;
        inputDate(returnDate, "Enter return date (dd mm yyyy): ");

        int delay = daysBetween(rec->dueDate, returnDate);
        if (delay > 0) {
            int fine = delay * finePerDay;
            cout << "Book is returned late.\n";
            cout << "Due date was: ";
            printDate(rec->dueDate);
            cout << "\nReturned on: ";
            printDate(returnDate);
            cout << "\nDays late: " << delay
                 << " | Fine: " << fine << " units.\n";
        } else {
            cout << "Book returned on time. No fine.\n";
        }
        delete rec;

        string nextStudent;
        if (b->dequeueWait(nextStudent)) {
            cout << "Next student in queue is: " << nextStudent << "\n";
            Date issueDate = returnDate;
            Date due = addDays(issueDate, loanDays);
            b->addIssued(nextStudent, issueDate, due);
            cout << "Book automatically issued to " << nextStudent << ".\n";
            cout << "New due date: ";
            printDate(due);
            cout << "\n";
        } else {
            b->availableCopies++;
        }
    }

    void deleteBookInteractive() {
        int id;
        cout << "Enter Book ID to delete: ";
        cin >> id;
        deleteBookById(id);
    }
};

#endif // LIBRARY_H
