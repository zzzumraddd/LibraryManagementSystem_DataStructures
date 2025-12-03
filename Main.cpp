#include <iostream>
#include "Library.h"
#include "User.h"
using namespace std;

void adminMenu(Library &lib, AuthSystem &auth) {
    int choice;
    do {
        cout << "\n===== ADMIN MENU =====\n";
        cout << "1. Add new book\n";
        cout << "2. Delete book\n";
        cout << "3. Display all books\n";
        cout << "4. Search books\n";
        cout << "5. Register new student\n";
        cout << "0. Save & logout\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: lib.addBookInteractive(); break;
            case 2: lib.deleteBookInteractive(); break;
            case 3: lib.displayAll(); break;
            case 4: lib.searchMenu(); break;
            case 5: auth.registerStudent(); break;
            case 0: lib.saveToFile(); cout << "Changes saved.\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

void librarianMenu(Library &lib) {
    int choice;
    do {
        cout << "\n===== LIBRARIAN MENU =====\n";
        cout << "1. Add new book\n";
        cout << "2. Issue book to student\n";
        cout << "3. Return book\n";
        cout << "4. Display all books\n";
        cout << "5. Search books\n";
        cout << "0. Save & logout\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: lib.addBookInteractive(); break;
            case 2: {
                string sid;
                cout << "Enter Student ID: ";
                cin >> sid;
                lib.issueBook(sid);
                break;
            }
            case 3: lib.returnBook(); break;
            case 4: lib.displayAll(); break;
            case 5: lib.searchMenu(); break;
            case 0: lib.saveToFile(); cout << "Changes saved.\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

void studentMenu(Library &lib, const User &user) {
    int choice;
    do {
        cout << "\n===== STUDENT MENU (" << user.username << ") =====\n";
        cout << "1. Search books\n";
        cout << "2. Request / Issue book\n";
        cout << "3. Return book\n";
        cout << "4. View all books (read-only)\n";
        cout << "0. Logout\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: lib.searchMenu(); break;
            case 2: lib.issueBook(user.username); break;
            case 3: lib.returnBook(user.username); break;
            case 4: lib.displayAll(); break;
            case 0: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

int main() {
    cout << "==== DATA STRUCTURES PROJECT: LIBRARY MANAGEMENT SYSTEM ====\n";
    cout << "Linked List + Queue + File Handling + Login + Due Dates/Fines\n\n";

    AuthSystem auth;
    auth.ensureDefaultUsers();

    Library lib;
    lib.loadFromFile();

    while (true) {
        User currentUser;
        cout << "\n1. Login\n";
        cout << "2. Exit program\n";
        cout << "Choice: ";
        int mainChoice;
        cin >> mainChoice;

        if (mainChoice == 2) {
            lib.saveToFile();
            cout << "Goodbye!\n";
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
            cout << "Invalid choice.\n";
        }
    }

    return 0;
}
