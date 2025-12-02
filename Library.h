#ifndef LIBRARY_H
#define LIBRARY_H

#include "Book.h"
#include <fstream>
#include <limits>

class Library {
    BookNode *head;
    std::string dbFile;
    int loanDays;
    int finePerDay;

public:
    Library(const std::string &file = "books.txt")
            : head(NULL), dbFile(file), loanDays(14), finePerDay(1000) {}

    ~Library() {
        freeBookList(head);
    }

    // ---------- FILE I/O ----------
    void loadFromFile() {
        std::ifstream fin(dbFile.c_str());
        if (!fin) {
            std::cout << "No existing book database found, starting empty.\n";
            return;
        }
        std::string line;
        while (std::getline(fin, line)) {
            if (line.empty()) continue;
            BookNode *node = BookNode::fromFileLine(line);
            if (node) insertSorted(node);
        }
        fin.close();
    }

    void saveToFile() {
        std::ofstream fout(dbFile.c_str());
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
            std::cout << "Book deleted.\n";
            return;
        }
        BookNode *cur = head;
        BookNode *prev = NULL;
        while (cur != NULL && cur->id != id) {
            prev = cur;
            cur = cur->next;
        }
        if (cur == NULL) {
            std::cout << "Book not found.\n";
            return;
        }
        prev->next = cur->next;
        cur->next = NULL;
        freeBookList(cur);
        std::cout << "Book deleted.\n";
    }

    // ---------- CORE FEATURES ----------
    void addBookInteractive() {
        int id, total;
        std::string title, author;

        std::cout << "Enter Book ID (integer): ";
        std::cin >> id;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid ID.\n";
            return;
        }
        if (existsId(id)) {
            std::cout << "Book with this ID already exists.\n";
            return;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Enter title: ";
        std::getline(std::cin, title);
        std::cout << "Enter author: ";
        std::getline(std::cin, author);

        std::cout << "Total copies: ";
        std::cin >> total;
        if (total <= 0) {
            std::cout << "Total copies must be positive.\n";
            return;
        }
        int available = total;

        BookNode *node = new BookNode(id, title, author, total, available);
        insertSorted(node);
        std::cout << "Book added successfully.\n";
    }

    void searchMenu() const {
        if (head == NULL) {
            std::cout << "No books in library.\n";
            return;
        }
        int choice;
        std::cout << "\nSearch by:\n";
        std::cout << "1. Book ID\n";
        std::cout << "2. Title\n";
        std::cout << "3. Author\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        if (choice == 1) {
            int id;
            std::cout << "Enter ID: ";
            std::cin >> id;
            BookNode *b = findById(id);
            if (b == NULL) std::cout << "Book not found.\n";
            else printBookDetails(b);
        } else if (choice == 2) {
            std::string q;
            std::cout << "Enter title keyword: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::getline(std::cin, q);
            searchByTitle(q);
        } else if (choice == 3) {
            std::string q;
            std::cout << "Enter author keyword: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::getline(std::cin, q);
            searchByAuthor(q);
        } else {
            std::cout << "Invalid choice.\n";
        }
    }

    void searchByTitle(const std::string &q) const {
        BookNode *cur = head;
        bool found = false;
        while (cur != NULL) {
            if (cur->title.find(q) != std::string::npos) {
                printBookDetails(cur);
                found = true;
            }
            cur = cur->next;
        }
        if (!found) std::cout << "No books with given title.\n";
    }

    void searchByAuthor(const std::string &q) const {
        BookNode *cur = head;
        bool found = false;
        while (cur != NULL) {
            if (cur->author.find(q) != std::string::npos) {
                printBookDetails(cur);
                found = true;
            }
            cur = cur->next;
        }
        if (!found) std::cout << "No books with given author.\n";
    }

    void printBookDetails(BookNode *b) const {
        std::cout << "-----------------------------\n";
        std::cout << "Book ID: " << b->id << "\n";
        std::cout << "Title: " << b->title << "\n";
        std::cout << "Author: " << b->author << "\n";
        std::cout << "Total copies: " << b->totalCopies << "\n";
        std::cout << "Available copies: " << b->availableCopies << "\n";
        std::cout << "In waiting queue: " << b->waitingCount() << "\n";
        int cnt = 0;
        IssuedRecord *ir = b->issuedHead;
        while (ir != NULL) {
            ++cnt;
            ir = ir->next;
        }
        std::cout << "Currently issued: " << cnt << "\n";
        std::cout << "-----------------------------\n";
    }

    void displayAll() const {
        if (head == NULL) {
            std::cout << "No books in library.\n";
            return;
        }
        std::cout << "\n======= All Books =======\n";
        BookNode *cur = head;
        while (cur != NULL) {
            cur->printBrief();
            cur = cur->next;
        }
        std::cout << "=========================\n";
    }

    void issueBook(const std::string &studentId) {
        int id;
        std::cout << "Enter Book ID to issue: ";
        std::cin >> id;
        BookNode *b = findById(id);
        if (b == NULL) {
            std::cout << "Book not found.\n";
            return;
        }

        if (b->findIssued(studentId) != NULL) {
            std::cout << "You already have this book issued.\n";
            return;
        }

        if (b->availableCopies > 0) {
            Date issueDate;
            inputDate(issueDate, "Enter issue date (dd mm yyyy): ");
            Date due = addDays(issueDate, loanDays);
            b->addIssued(studentId, issueDate, due);
            b->availableCopies--;

            std::cout << "Book issued successfully.\n";
            std::cout << "Due date: ";
            printDate(due);
            std::cout << "\n";
        } else {
            if (b->isStudentInQueue(studentId)) {
                std::cout << "You are already in the waiting queue.\n";
                return;
            }
            b->enqueueWait(studentId);
            int pos = b->waitingCount();
            std::cout << "No copies available now. You are added to waiting list.\n";
            std::cout << "Your position in queue: " << pos << "\n";
        }
    }

    void returnBook(const std::string &studentIdOpt = "") {
        int id;
        std::cout << "Enter Book ID to return: ";
        std::cin >> id;
        BookNode *b = findById(id);
        if (b == NULL) {
            std::cout << "Book not found.\n";
            return;
        }

        std::string studentId = studentIdOpt;
        if (studentId.empty()) {
            std::cout << "Enter Student ID who is returning: ";
            std::cin >> studentId;
        }

        IssuedRecord *rec = NULL;
        if (!b->removeIssued(studentId, rec)) {
            std::cout << "This student does not have this book.\n";
            return;
        }

        Date returnDate;
        inputDate(returnDate, "Enter return date (dd mm yyyy): ");

        int delay = daysBetween(rec->dueDate, returnDate);
        if (delay > 0) {
            int fine = delay * finePerDay;
            std::cout << "Book is returned late.\n";
            std::cout << "Due date was: ";
            printDate(rec->dueDate);
            std::cout << "\nReturned on: ";
            printDate(returnDate);
            std::cout << "\nDays late: " << delay
                      << " | Fine: " << fine << " units.\n";
        } else {
            std::cout << "Book returned on time. No fine.\n";
        }
        delete rec;

        std::string nextStudent;
        if (b->dequeueWait(nextStudent)) {
            std::cout << "Next student in queue is: " << nextStudent << "\n";
            Date issueDate = returnDate;
            Date due = addDays(issueDate, loanDays);
            b->addIssued(nextStudent, issueDate, due);
            std::cout << "Book automatically issued to " << nextStudent << ".\n";
            std::cout << "New due date: ";
            printDate(due);
            std::cout << "\n";
        } else {
            b->availableCopies++;
        }
    }

    void deleteBookInteractive() {
        int id;
        std::cout << "Enter Book ID to delete: ";
        std::cin >> id;
        deleteBookById(id);
    }
};

#endif // LIBRARY_H
