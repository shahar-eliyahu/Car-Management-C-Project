# Car Management System (C Project)

## Overview
This project was developed as part of a university course in C programming.  
It is a **Car Management System** that runs as a command-line application (CLI).  
The system demonstrates structured programming in C with modular design, file persistence, and management of entities like cars, customers, parts, and garages.

## Features
- **Car Management** – Add, update, and view cars.
- **Customer Management** – Manage customer details and ownership of cars.
- **Garage Management** – Track garage operations and assignments.
- **Parts Inventory** – Add and manage parts.
- **File Persistence** – Data is saved and loaded from text/binary files.
- **Menu System** – Interactive CLI menu for all operations.
- **Error Handling** – Validation of inputs and basic exception handling in C.

## Project Structure

```
car.c / car.h                         # Car entity and related operations
customer.c / customer.h               # Customer entity and related operations
garage.c / garage.h                   # Garage entity and logic
part.c / part.h                       # Parts inventory management
store_manager.c / store_manager.h     # Main store/management functions
menu.c / menu.h                       # CLI menu system
file_operations.c / file_operations.h # File saving/loading logic
constants.h                           # Global constants
main.c                                # Program entry point
system_data.txt                       # Example data file (text format)
```

## How to Run
1. Open the project in **Visual Studio** or any C IDE.  

2. Compile all `.c` files together:  

    ```bash
    gcc *.c -o car_management
    ```

3. Run the program:  

    ```bash
    ./car_management
    ```

4. Follow the on-screen menu to interact with the system.


## Learning Outcomes
- Proficiency in **modular C programming** using headers and source files.  
- Experience with **file I/O** in C (text and binary formats).  
- Designing structured systems with multiple components (cars, customers, parts).  
- Working with pointers, dynamic memory, and modular compilation.


## Notes
- The `system_data.txt` file is included as a sample data file.  
- IDE-specific files (like `.sln` or `.vcxproj`) are excluded with `.gitignore`.  
- This project is intended for educational purposes. 
