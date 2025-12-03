#ifndef USER_H
#define USER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

enum Role {
    ROLE_ADMIN,
    ROLE_LIBRARIAN,
    ROLE_STUDENT
};

struct User {
    string username;
    string password;
    Role role;
};

inline string roleToString(Role r) {
    switch (r) {
        case ROLE_ADMIN:     return "ADMIN";
        case ROLE_LIBRARIAN: return "LIBRARIAN";
        case ROLE_STUDENT:   return "STUDENT";
        default:             return "STUDENT";
    }
}

inline Role stringToRole(const string &s) {
    if (s == "ADMIN")     return ROLE_ADMIN;
    if (s == "LIBRARIAN") return ROLE_LIBRARIAN;
    return ROLE_STUDENT;
}

class AuthSystem {
    string filename;
public:
    explicit AuthSystem(const string &file = "users.txt")
            : filename(file) {}

    // Create default users file if it does not exist
    void ensureDefaultUsers() {
        ifstream fin(filename.c_str());
        if (fin.good()) return;
        fin.close();

        ofstream fout(filename.c_str());
        // username|password|role
        fout << "admin|admin|ADMIN\n";
        fout << "lib|lib|LIBRARIAN\n";
        fout << "std|std|STUDENT\n";
        fout.close();

        cout << "Created default users file: " << filename << "\n";
        cout << "Admin: admin/admin, Librarian: lib/lib, Student: std/std\n";
    }

    bool loadAllUsers(vector<User> &out) {
        out.clear();
        ifstream fin(filename.c_str());
        if (!fin) return false;
        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string uname, pass, rstr;
            getline(ss, uname, '|');
            getline(ss, pass, '|');
            getline(ss, rstr, '|');
            if (uname.empty()) continue;
            User u;
            u.username = uname;
            u.password = pass;
            u.role = stringToRole(rstr);
            out.push_back(u);
        }
        return true;
    }

    bool login(User &outUser) {
        vector<User> users;
        if (!loadAllUsers(users)) {
            cout << "Cannot open users file.\n";
            return false;
        }
        string uname, pass;
        cout << "Username: ";
        cin >> uname;
        cout << "Password: ";
        cin >> pass;

        for (size_t i = 0; i < users.size(); ++i) {
            const User &u = users[i];
            if (u.username == uname && u.password == pass) {
                outUser = u;
                cout << "Logged in as " << uname
                     << " (" << roleToString(u.role) << ")\n";
                return true;
            }
        }
        cout << "Invalid username or password.\n";
        return false;
    }

    // Admin can register a new student user
    void registerStudent() {
        vector<User> users;
        loadAllUsers(users);

        string uname, pass;
        cout << "Enter new student username (used as Student ID): ";
        cin >> uname;

        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].username == uname) {
                cout << "User already exists.\n";
                return;
            }
        }
        cout << "Enter password: ";
        cin >> pass;

        ofstream fout(filename.c_str(), ios::app);
        fout << uname << "|" << pass << "|STUDENT\n";
        fout.close();
        cout << "Student registered successfully.\n";
    }
};

#endif // USER_H
