# Library Management System (Data Structures Project)

INHA University in Tashkent  
School of Computer and Information Engineering  
Course: Data Structures  
Semester: Fall 2025  

## 1. Project Overview

This project is a console-based **Library Management System (LMS)** implemented in C++.  
The main goal is to practice **Data Structures principles** using:

- Singly linked lists
- Queues
- File handling
- Basic modular design with header files

The system allows:

- Managing books in a library (add, delete, search, display)
- Issuing and returning books
- Maintaining a **waiting list** for unavailable books
- Handling **multiple user roles** (Admin, Librarian, Student)
- Calculating **due dates and fines** for late returns

Everything is implemented using only standard C++ and the STL (no external libraries).

---

## 2. Implemented Requirements

### 2.1 Core Requirements

1. **Book Management (Linked List)**
   - Each book is stored as a node (`BookNode`) in a **singly linked list**.
   - Fields: `id`, `title`, `author`, `totalCopies`, `availableCopies`.
   - Operations:
     - Add new book (insert in sorted order by `id`)
     - Delete book by `id`
     - Display all books
     - Search by:
       - Book ID  
       - Title (substring match)  
       - Author (substring match)

2. **Issue and Return Books**
   - A student can request a book by ID.
   - If copies are available:
     - Book is issued.
     - A due date is assigned.
   - If no copies are available:
     - Student is added to a **waiting queue** for that book.
   - When a book is returned:
     - If there is a waiting queue, the next student automatically receives the book.
     - Otherwise, the available copy count is increased.

3. **Waiting List (Queue)**
   - For each book, there is a **FIFO queue** of students waiting (`WaitNode`).
   - Implemented as a linked queue with `waitFront` / `waitRear` pointers.
   - Operations:
     - Enqueue student ID (`O(1)`)
     - Dequeue next student (`O(1)`)
     - Prevent adding the same student twice into the same queue.

4. **File Handling**
   - Books are saved in `books.txt` when exiting Admin/Librarian menus.
   - Format for each book:
     ```text
     id|title|author|totalCopies|availableCopies
     ```
   - On startup, the program loads books from `books.txt` if it exists.
   - Users (logins) are stored in `users.txt`.

---

## 3. Extended Features (B & C)

This project implements the following **Extended Features**:

### B. User Login System (Role-Based Access)

Three user roles:

- **Admin**
  - Add new books
  - Delete books
  - Display all books
  - Search books
  - Register new student users

- **Librarian**
  - Add new books
  - Issue books to students
  - Return books (for any student)
  - Display all books
  - Search books

- **Student**
  - Search books
  - Request / issue book (for themselves)
  - Return their own books
  - View all books (read-only)

Login credentials are stored in `users.txt` in the format:

```text
username|password|ROLE

