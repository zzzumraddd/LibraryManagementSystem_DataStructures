#ifndef USER_H
#define USER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// We use a simple enum (C++98 compatible)
enum Role {
    ROLE_ADMIN,
    ROLE_LIBRARIAN,
    ROLE_STUDENT
};

struct User {
    std::string username;
    std::string password;
    Role role;
};

inline std::string roleToString(Role r) {
    switch (r) {
        case ROLE_ADMIN:     return "ADMIN";
        case ROLE_LIBRARIAN: return "LIBRARIAN";
        case ROLE_STUDENT:   return "STUDENT";
        default:             return "STUDENT";
    }
}

inline Role stringToRole(const std::string &s) {
    if (s == "ADMIN")     return ROLE_ADMIN;
    if (s == "LIBRARIAN") return ROLE_LIBRARIAN;
    return ROLE_STUDENT;
}

class AuthSystem {
    std::string filename;
public:
    explicit AuthSystem(const std::string &file = "users.txt")
            : filename(file) {}

    // Create default users file if it does not exist
    void ensureDefaultUsers() {
        std::ifstream fin(filename.c_str());
        if (fin.good()) return;
        fin.close();

        std::ofstream fout(filename.c_str());
        // username|password|role
        fout << "admin|admin|ADMIN\n";
        fout << "lib|lib|LIBRARIAN\n";
        fout << "std|std|STUDENT\n";
        fout.close();

        std::cout << "Created default users file: " << filename << "\n";
        std::cout << "Admin: admin/admin, Librarian: lib/lib, Student: std/std\n";
    }

    bool loadAllUsers(std::vector<User> &out) {
        out.clear();
        std::ifstream fin(filename.c_str());
        if (!fin) return false;
        std::string line;
        while (std::getline(fin, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string uname, pass, rstr;
            std::getline(ss, uname, '|');
            std::getline(ss, pass, '|');
            std::getline(ss, rstr, '|');
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
        std::vector<User> users;
        if (!loadAllUsers(users)) {
            std::cout << "Cannot open users file.\n";
            return false;
        }
        std::string uname, pass;
        std::cout << "Username: ";
        std::cin >> uname;
        std::cout << "Password: ";
        std::cin >> pass;

        for (size_t i = 0; i < users.size(); ++i) {
            const User &u = users[i];
            if (u.username == uname && u.password == pass) {
                outUser = u;
                std::cout << "Logged in as " << uname
                          << " (" << roleToString(u.role) << ")\n";
                return true;
            }
        }
        std::cout << "Invalid username or password.\n";
        return false;
    }

    // Admin can register a new student user
    void registerStudent() {
        std::vector<User> users;
        loadAllUsers(users);

        std::string uname, pass;
        std::cout << "Enter new student username (used as Student ID): ";
        std::cin >> uname;

        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].username == uname) {
                std::cout << "User already exists.\n";
                return;
            }
        }
        std::cout << "Enter password: ";
        std::cin >> pass;

        std::ofstream fout(filename.c_str(), std::ios::app);
        fout << uname << "|" << pass << "|STUDENT\n";
        fout.close();
        std::cout << "Student registered successfully.\n";
    }
};

#endif // USER_H
