#include <iostream>
#include "Library.h"
#include "User.h"

void adminMenu(Library &lib, AuthSystem &auth) {
    int choice;
    do {
        std::cout << "\n===== ADMIN MENU =====\n";
        std::cout << "1. Add new book\n";
        std::cout << "2. Delete book\n";
        std::cout << "3. Display all books\n";
        std::cout << "4. Search books\n";
        std::cout << "5. Register new student\n";
        std::cout << "0. Save & logout\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: lib.addBookInteractive(); break;
            case 2: lib.deleteBookInteractive(); break;
            case 3: lib.displayAll(); break;
            case 4: lib.searchMenu(); break;
            case 5: auth.registerStudent(); break;
            case 0: lib.saveToFile(); std::cout << "Changes saved.\n"; break;
            default: std::cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

void librarianMenu(Library &lib) {
    int choice;
    do {
        std::cout << "\n===== LIBRARIAN MENU =====\n";
        std::cout << "1. Add new book\n";
        std::cout << "2. Issue book to student\n";
        std::cout << "3. Return book\n";
        std::cout << "4. Display all books\n";
        std::cout << "5. Search books\n";
        std::cout << "0. Save & logout\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: lib.addBookInteractive(); break;
            case 2: {
                std::string sid;
                std::cout << "Enter Student ID: ";
                std::cin >> sid;
                lib.issueBook(sid);
                break;
            }
            case 3: lib.returnBook(); break;
            case 4: lib.displayAll(); break;
            case 5: lib.searchMenu(); break;
            case 0: lib.saveToFile(); std::cout << "Changes saved.\n"; break;
            default: std::cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

void studentMenu(Library &lib, const User &user) {
    int choice;
    do {
        std::cout << "\n===== STUDENT MENU (" << user.username << ") =====\n";
        std::cout << "1. Search books\n";
        std::cout << "2. Request / Issue book\n";
        std::cout << "3. Return book\n";
        std::cout << "4. View all books (read-only)\n";
        std::cout << "0. Logout\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: lib.searchMenu(); break;
            case 2: lib.issueBook(user.username); break;
            case 3: lib.returnBook(user.username); break;
            case 4: lib.displayAll(); break;
            case 0: std::cout << "Logging out...\n"; break;
            default: std::cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

int main() {
    std::cout << "==== DATA STRUCTURES PROJECT: LIBRARY MANAGEMENT SYSTEM ====\n";
    std::cout << "Linked List + Queue + File Handling + Login + Due Dates/Fines\n\n";

    AuthSystem auth;
    auth.ensureDefaultUsers();

    Library lib;
    lib.loadFromFile();

    while (true) {
        User currentUser;
        std::cout << "\n1. Login\n";
        std::cout << "2. Exit program\n";
        std::cout << "Choice: ";
        int mainChoice;
        std::cin >> mainChoice;

        if (mainChoice == 2) {
            lib.saveToFile();
            std::cout << "Goodbye!\n";
            break;
        }

        if (mainChoice == 1) {
            if (!auth.login(currentUser)) {
                continue;
            }
            if (currentUser.role == ROLE_ADMIN) {
                adminMenu(lib, auth);
            } else if (currentUser.role == ROLE_LIBRARIAN) {
                librarianMenu(lib);
            } else {
                studentMenu(lib, currentUser);
            }
        } else {
            std::cout << "Invalid choice.\n";
        }
    }

    return 0;
}
