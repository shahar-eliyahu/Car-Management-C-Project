#define _CRT_SECURE_NO_WARNINGS
#include "menu.h"
#include "file_operations.h"
#include "car.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void handleAddMenu(StoreManager* storeManager) {
    int choice;
    char input[INPUT_BUFFER_SIZE];
    char type[MAX_TYPE_LEN], engine_type[MAX_ENGINE_TYPE_LEN], brand[MAX_BRAND_LEN], model[MAX_MODEL_LEN], name[MAX_NAME_LEN], manufacturer[MAX_NAME_LEN];
    float price, length, width, height;

    printf("Add Menu:\n");
    printf("1. Add Car\n");
    printf("2. Add Part\n");
    printf("3. Back to Main Menu\n");
    printf("Please select an option: ");
    fgets(input, INPUT_BUFFER_SIZE, stdin);
    if (sscanf(input, "%d", &choice) != 1 || choice < 1 || choice > 3) {
        printf("Invalid input. Please enter a number between 1 and 3. Returning to main menu.\n");
        return;
    }

    switch (choice) {
    case 1:
        printf("Enter car type: ");
        fgets(type, MAX_TYPE_LEN, stdin);
        type[strcspn(type, "\n")] = 0;
        if (strlen(type) == 0) {
            printf("Invalid input for car type. Returning to main menu.\n");
            return;
        }

        printf("Enter engine type: ");
        fgets(engine_type, MAX_ENGINE_TYPE_LEN, stdin);
        engine_type[strcspn(engine_type, "\n")] = 0;
        if (strlen(engine_type) == 0) {
            printf("Invalid input for engine type. Returning to main menu.\n");
            return;
        }

        printf("Enter brand: ");
        fgets(brand, MAX_BRAND_LEN, stdin);
        brand[strcspn(brand, "\n")] = 0;
        if (strlen(brand) == 0) {
            printf("Invalid input for brand. Returning to main menu.\n");
            return;
        }

        printf("Enter model: ");
        fgets(model, MAX_MODEL_LEN, stdin);
        model[strcspn(model, "\n")] = 0;
        if (strlen(model) == 0) {
            printf("Invalid input for model. Returning to main menu.\n");
            return;
        }

        printf("Enter price: ");
        fgets(input, INPUT_BUFFER_SIZE, stdin);
        if (sscanf(input, "%f", &price) != 1 || price < 0) {
            printf("Invalid input for price. Must be a positive number. Returning to main menu.\n");
            return;
        }

        Car* car = createCar(type, engine_type, brand, model, price, &storeManager->carInventory);
        if (car) {
            addCarToInventory(&storeManager->carInventory, car);
            printf("Car added successfully.\n");
        }
        else {
            printf("Failed to create car. Returning to main menu.\n");
        }
        break;

    case 2:
        printf("Enter part name: ");
        fgets(name, MAX_NAME_LEN, stdin);
        name[strcspn(name, "\n")] = 0;
        if (strlen(name) == 0) {
            printf("Invalid input for part name. Returning to main menu.\n");
            return;
        }

        printf("Enter brand: ");
        fgets(brand, MAX_BRAND_LEN, stdin);
        brand[strcspn(brand, "\n")] = 0;
        if (strlen(brand) == 0) {
            printf("Invalid input for brand. Returning to main menu.\n");
            return;
        }

        printf("Enter price: ");
        fgets(input, INPUT_BUFFER_SIZE, stdin);
        if (sscanf(input, "%f", &price) != 1 || price < 0) {
            printf("Invalid input for price. Must be a positive number. Returning to main menu.\n");
            return;
        }

        printf("Enter manufacturer: ");
        fgets(manufacturer, MAX_NAME_LEN, stdin);
        manufacturer[strcspn(manufacturer, "\n")] = 0;
        if (strlen(manufacturer) == 0) {
            printf("Invalid input for manufacturer. Returning to main menu.\n");
            return;
        }

        printf("Enter dimensions separated by a space (length width height): ");
        fgets(input, INPUT_BUFFER_SIZE, stdin);
        char extra;
        int count = sscanf(input, "%f %f %f %c", &length, &width, &height, &extra);
        if (count != 3 || length <= 0 || width <= 0 || height <= 0) {
            printf("Invalid input for dimensions. Please enter exactly three positive numbers separated by spaces. Returning to main menu.\n");
            return;
        }

        Part* part = createPart(name, brand, price, manufacturer, length, width, height, &storeManager->partInventory);
        if (part) {
            addPartToInventory(&storeManager->partInventory, part);
            printf("Part added successfully.\n");
        }
        else {
            printf("Failed to create part. Returning to main menu.\n");
        }
        break;

    case 3:
        return;

    default:
        printf("Invalid choice. Returning to main menu.\n");
        break;
    }
}

void displayMenu(StoreManager* storeManager) {
    int choice;
    char input[INPUT_BUFFER_SIZE];

    do {
        printf("***********************************************\n");
        printf("Welcome to the Car Market System\n");
        printf("1. Load System from File\n");
        printf("2. Save System to File\n");
        printf("3. Display All Cars\n");
        printf("4. Display All Parts\n");
        printf("5. Add Items to System\n");
        printf("6. Remove Items from System\n");
        printf("7. Sort Items in System\n");
        printf("8. Search Items in System\n");
        printf("9. Add Customer\n");
        printf("10. Purchase Products\n");
        printf("11. Print All Customers\n");
        printf("12. Display All Transactions\n");
        printf("13. Increase Car Prices by 15 percent \n");
        printf("14. Manage Garages\n"); // New option for managing garages
        printf("15. Exit\n");
        printf("Please select an option: \n");
        printf("***********************************************\n");

        fgets(input, INPUT_BUFFER_SIZE, stdin);
        if (sscanf(input, "%d", &choice) != 1) {
            printf("Invalid input. Please enter a number between 1 and 15.\n");
            continue;
        }

        switch (choice) {
        case 1:
            handleLoadSystem(storeManager);
            break;
        case 2:
            handleSaveSystem(storeManager);
            break;
        case 3:
            printAllCars(&storeManager->carInventory);
            break;
        case 4:
            printAllParts(&storeManager->partInventory);
            break;
        case 5:
            handleAddMenu(storeManager);
            break;
        case 6:
            handleRemoveMenu(storeManager);
            break;
        case 7:
            handleSortMenu(storeManager);
            break;
        case 8:
            handleSearchMenu(storeManager);
            break;
        case 9:
            handleAddCustomer(storeManager);
            break;
        case 10:
            handlePurchaseMenu(storeManager);
            break;
        case 11:
            printAllCustomers(&storeManager->customerList);
            break;
        case 12:
            displayAllTransactions(storeManager);
            break;
        case 13:
            handleIncreaseCarPrices(storeManager);
            break;
        case 14:
            handleManageGarageMenu(storeManager); 
            break;
        case 15:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    } while (choice != 15);
}



void handleLoadSystem(StoreManager* storeManager) {
    int isBinary;
    char input[INPUT_BUFFER_SIZE];

    printf("From what file do you want to load? (1 for Bin, 0 for txt): ");
    fgets(input, INPUT_BUFFER_SIZE, stdin);
    if (sscanf(input, "%d", &isBinary) != 1 || (isBinary != 0 && isBinary != 1)) {
        printf("Invalid input. Please enter 1 or 0.\n");
        return;
    }
    if (isBinary == 1) {
        if (loadFromFile("system_data.bin", storeManager, isBinary) == 0) {
            printf("Operation failed!\n");
        }
    }
    else {
        if (loadFromFile("system_data.txt", storeManager, isBinary) == 0) {
            printf("Operation failed!\n");
        }
    }
}

void handleSaveSystem(StoreManager* storeManager) {
    int isBinary;
    char input[INPUT_BUFFER_SIZE];

    printf("Save as binary file? (1 for Yes, 0 for No): ");
    fgets(input, INPUT_BUFFER_SIZE, stdin);
    if (sscanf(input, "%d", &isBinary) != 1 || (isBinary != 0 && isBinary != 1)) {
        printf("Invalid input. Please enter 1 or 0.\n");
        return;
    }
    if (isBinary == 1) {
        if (saveToFile("system_data.bin", storeManager, isBinary) == 0) {
            printf("Operation failed!\n");
        }
    }
    else {
        if (saveToFile("system_data.txt", storeManager, isBinary) == 0) {
            printf("Operation failed!\n");
        }
    }
}

void handleRemoveMenu(StoreManager* storeManager) {
    int choice;
    char input[INPUT_BUFFER_SIZE];
    int serial_number;

    printf("Remove Menu:\n");
    printf("1. Remove Car\n");
    printf("2. Remove Part\n");
    printf("3. Back to Main Menu\n");
    printf("Please select an option: ");
    fgets(input, INPUT_BUFFER_SIZE, stdin);
    if (sscanf(input, "%d", &choice) != 1 || choice < 1 || choice > 3) {
        printf("Invalid input. Please enter a number between 1 and 3. Returning to main menu.\n");
        return;
    }

    switch (choice) {
    case 1:
        if (storeManager->carInventory.count == 0) {
            printf("No cars available in the inventory to remove.\n");
            return;
        }
        printAllCars(&storeManager->carInventory);
        printf("Enter car serial number to remove: ");
        fgets(input, INPUT_BUFFER_SIZE, stdin);
        if (sscanf(input, "%d", &serial_number) != 1) {
            printf("Invalid serial number. Returning to main menu.\n");
            return;
        }
        if (removeCarFromInventory(&storeManager->carInventory, serial_number)) {
            printf("Car removed successfully.\n");
        }
        else {
            printf("Car not found. No car removed.\n");
        }
        break;
    case 2:
        if (storeManager->partInventory.count == 0) {
            printf("No parts available in the inventory to remove.\n");
            return;
        }
        printAllParts(&storeManager->partInventory);
        printf("Enter part serial number to remove: ");
        fgets(input, INPUT_BUFFER_SIZE, stdin);
        if (sscanf(input, "%d", &serial_number) != 1) {
            printf("Invalid serial number. Returning to main menu.\n");
            return;
        }
        if (removePartFromInventory(&storeManager->partInventory, serial_number)) {
            printf("Part removed successfully.\n");
        }
        else {
            printf("Part not found. No part removed.\n");
        }
        break;
    case 3:
        return;
    default:
        printf("Invalid choice. Returning to main menu.\n");
        break;
    }
}

void handleSortMenu(StoreManager* storeManager) {
    int choice;
    char input[INPUT_BUFFER_SIZE];

    printf("Sort Menu:\n");
    printf("1. Sort Cars by Price\n");
    printf("2. Sort Cars by Brand\n");
    printf("3. Sort Cars by Serial Number\n");
    printf("4. Sort Parts by Price\n");
    printf("5. Sort Parts by Brand\n");
    printf("6. Back to Main Menu\n");
    printf("Please select an option: ");
    fgets(input, INPUT_BUFFER_SIZE, stdin);
    if (sscanf(input, "%d", &choice) != 1) {
        printf("Invalid input. Returning to main menu.\n");
        return;
    }

    switch (choice) {
    case 1:
        if (storeManager->carInventory.count == 0) {
            printf("No cars available in the inventory to sort.\n");
            return;
        }
        sortCars(&storeManager->carInventory, compareByPrice);
        printf("Cars sorted by price.\n");
        break;
    case 2:
        if (storeManager->carInventory.count == 0) {
            printf("No cars available in the inventory to sort.\n");
            return;
        }
        sortCars(&storeManager->carInventory, compareByBrand);
        printf("Cars sorted by brand.\n");
        break;
    case 3:
        if (storeManager->carInventory.count == 0) {
            printf("No cars available in the inventory to sort.\n");
            return;
        }
        sortCars(&storeManager->carInventory, compareBySerialNumber);
        printf("Cars sorted by serial number.\n");
        break;
    case 4:
        if (storeManager->partInventory.count == 0) {
            printf("No parts available in the inventory to sort.\n");
            return;
        }
        sortParts(&storeManager->partInventory, comparePartsByPrice);
        printf("Parts sorted by price.\n");
        break;
    case 5:
        if (storeManager->partInventory.count == 0) {
            printf("No parts available in the inventory to sort.\n");
            return;
        }
        sortParts(&storeManager->partInventory, comparePartsByBrand);
        printf("Parts sorted by brand.\n");
        break;
    case 6:
        return;
    default:
        printf("Invalid choice. Returning to main menu.\n");
        break;
    }
}

void handleSearchMenu(StoreManager* storeManager) {
    int choice;
    char input[INPUT_BUFFER_SIZE];
    int serial_number;
    char brand[MAX_BRAND_LEN];
    float price;
    Car* car;

    printf("Search Menu:\n");
    printf("1. Search Cars by Serial Number\n");
    printf("2. Search Cars by Brand\n");
    printf("3. Search Cars by Price\n");
    printf("4. Back to Main Menu\n");
    printf("Please select an option: ");
    fgets(input, INPUT_BUFFER_SIZE, stdin);
    if (sscanf(input, "%d", &choice) != 1) {
        printf("Invalid input. Returning to main menu.\n");
        return;
    }

    switch (choice) {
    case 1:
        printf("Enter car serial number to search: ");
        fgets(input, INPUT_BUFFER_SIZE, stdin);
        if (sscanf(input, "%d", &serial_number) != 1) {
            printf("Invalid serial number. Returning to main menu.\n");
            return;
        }

        // sort by serial number before searching
        sortCars(&storeManager->carInventory, compareBySerialNumber);


        car = findCar(&storeManager->carInventory, &serial_number, searchBySerialNumber);
        if (car != NULL) {
            printCar(car);
        }
        else {
            printf("Car not found.\n");
        }
        break;

    case 2:
        printf("Enter car brand to search: ");
        fgets(brand, MAX_BRAND_LEN, stdin);
        brand[strcspn(brand, "\n")] = 0;

        // sort by brand before searching
        sortCars(&storeManager->carInventory, compareByBrand);

        // search by brand
        car = findCar(&storeManager->carInventory, brand, searchByBrand);
        if (car != NULL) {  // check if car was found
            printCar(car);
        }
        else {
            printf("Car not found.\n");
        }
        break;

    case 3:
        printf("Enter car price to search: ");
        fgets(input, INPUT_BUFFER_SIZE, stdin);
        if (sscanf(input, "%f", &price) != 1) {
            printf("Invalid price. Returning to main menu.\n");
            return;
        }

        // sort by price before searching
        sortCars(&storeManager->carInventory, compareByPrice);

        // search by price
        car = findCar(&storeManager->carInventory, &price, searchByPrice);
        if (car != NULL) {  //check if car was found
            printCar(car);
        }
        else {
            printf("Car not found.\n");
        }
        break;

    case 4:
        return;

    default:
        printf("Invalid choice. Returning to main menu.\n");
        break;
    }
}

void handleAddCustomer(StoreManager* storeManager) {
    char first_name[MAX_CUSTOMER_NAME_LEN], last_name[MAX_CUSTOMER_NAME_LEN], id[MAX_CUSTOMER_ID_LEN + 2];

    printf("Enter first name: ");
    fgets(first_name, MAX_CUSTOMER_NAME_LEN, stdin);
    first_name[strcspn(first_name, "\n")] = 0;

    printf("Enter last name: ");
    fgets(last_name, MAX_CUSTOMER_NAME_LEN, stdin);
    last_name[strcspn(last_name, "\n")] = 0;

    printf("Enter ID: ");
    fgets(id, sizeof(id), stdin);

    // Check if input is too long
    if (id[strlen(id) - 1] != '\n') {
        printf("ID must be exactly 9 digits. Returning to main menu.\n");

        // Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}

        return;
    }

    // Remove newline character
    id[strcspn(id, "\n")] = 0;

    // Check if ID is too short
    if (strlen(id) != MAX_CUSTOMER_ID_LEN) {
        printf("ID must be exactly 9 digits. Returning to main menu.\n");
        return;
    }

    //Check if all characters are digits
    for (int i = 0; i < MAX_CUSTOMER_ID_LEN; i++) {
        if (!isdigit(id[i])) {
            printf("ID must contain only digits. Returning to main menu.\n");
            return;
        }
    }

    // Check if customer already exists
    for (int i = 0; i < storeManager->customerList.count; i++) {
        if (strcmp(storeManager->customerList.customers[i]->id, id) == 0) {
            printf("Customer with ID %s already exists.\n", id);
            return;
        }
    }

    Customer* customer = createCustomer(first_name, last_name, id);
    if (customer && addCustomer(storeManager, customer)) {
        printf("Customer added successfully: ID = %s, Name = %s %s\n", id, first_name, last_name);
    }
    else {
        printf("Failed to add customer.\n");
    }
}


void handlePurchaseMenu(StoreManager* storeManager) {

    char id[MAX_CUSTOMER_ID_LEN + 2];
    int choice;
    char input[INPUT_BUFFER_SIZE];

    printAllCustomers(&storeManager->customerList);

    printf("Enter the ID of the customer who wants to purchase : ");
    fgets(id, sizeof(id), stdin);

    if (id[strlen(id) - 1] != '\n') {
        printf("ID must be exactly 9 digits. Returning to main menu.\n");

        // Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}

        return;
    }

    // Remove newline character
    id[strcspn(id, "\n")] = 0;

    // Check if ID is too short
    if (strlen(id) != MAX_CUSTOMER_ID_LEN) {
        printf("ID must be exactly 9 digits. Returning to main menu.\n");
        return;
    }

    //Check if all characters are digits
    for (int i = 0; i < MAX_CUSTOMER_ID_LEN; i++) {
        if (!isdigit(id[i])) {
            printf("ID must contain only digits. Returning to main menu.\n");
            return;
        }
    }

    Customer* customer = NULL;
    for (int i = 0; i < storeManager->customerList.count; i++) {
        if (strcmp(storeManager->customerList.customers[i]->id, id) == 0) {
            customer = storeManager->customerList.customers[i];
            break;
        }
    }

    if (!customer) {
        printf("Customer not found. Please make sure the ID is correct and exists in the system.\n");
        return;
    }

    printf("Purchase Menu:\n");
    printf("1. Buy Car\n");
    printf("2. Buy Part\n");
    printf("Please select an option: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    if (sscanf(input, "%d", &choice) != 1) {
        printf("Invalid input. Returning to main menu.\n");
        return;
    }

    int serial_number;
    switch (choice) {
    case 1:
        if (storeManager->carInventory.count == 0) {
            printf("No cars available in the inventory to purchase.\n");
            return;
        }

        printAllCars(&storeManager->carInventory);
        printf("Enter car serial number: ");
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%d", &serial_number) != 1) {
            printf("Invalid serial number. Returning to main menu.\n");
            return;
        }
        if (purchaseCar(storeManager, customer, serial_number)) {
            printf("Car purchased successfully.\n");
        }
        else {
            printf("Error: Car not found.\n");
        }
        break;
    case 2:
        if (storeManager->partInventory.count == 0) {
            printf("No parts available in the inventory to purchase.\n");
            return;
        }
        printAllParts(&storeManager->partInventory);
        printf("Enter part serial number: ");
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%d", &serial_number) != 1) {
            printf("Invalid serial number. Returning to main menu.\n");
            return;
        }
        if (purchasePart(storeManager, customer, serial_number)) {
            printf("Part purchased successfully.\n");
        }
        else {
            printf("Error: Part not found.\n");
        }
        break;
    default:
        printf("Invalid choice. Returning to main menu.\n");
        break;
    }
}

void displayAllTransactions(const StoreManager* storeManager) {
    printf("Displaying all transactions\n");

    int transactionsFound = 0;

    for (int i = 0; i < storeManager->customerList.count; i++) {
        Customer* customer = storeManager->customerList.customers[i];
        int customerHasTransactions = 0;

        printf("   -Customer ID: %s, Name: %s %s\n", customer->id, customer->first_name, customer->last_name);

        // Display purchased cars
        LinkedListNode* current = customer->purchased_cars->head;
        if (current) {
            printf("Purchased Cars:\n");
            customerHasTransactions = 1;
        }
        while (current) {
            Car* car = (Car*)current->data;
            printCar(car);
            current = current->next;
        }

        // Display purchased parts
        current = customer->purchased_parts->head;
        if (current) {
            printf("Purchased Parts:\n");
            customerHasTransactions = 1;
        }
        while (current) {
            Part* part = (Part*)current->data;
            printPart(part);
            current = current->next;
        }

        if (customerHasTransactions) {
            transactionsFound = 1;
        }

        printf("\n");
    }

    if (!transactionsFound) {
        printf("No transactions found.\n");
    }
}


void handleIncreaseCarPrices(StoreManager* storeManager) {
    if (storeManager->carInventory.count == 0) {
        printf("No cars available in the inventory to increase prices.\n");
        return;
    }

    // Increase all car prices by the defined percentage in the macro
    increaseAllCarPrices(&storeManager->carInventory);

    // Display the correct percentage increase
    printf("Car prices increased by %.2f%%.\n", PERCENTAGE_INCREASE_CAR_PRICES);
}
void handleManageGarageMenu(StoreManager* storeManager) {
    int choice;
    char input[INPUT_BUFFER_SIZE];
    Garage garage;

    do {
        printf("***********************************************\n");
        printf("Garage Management Menu\n");
        printf("1. Add Garage\n");
        printf("2. Remove Garage\n");
        printf("3. Display All Garages\n");
        printf("4. Back to Main Menu\n");
        printf("Please select an option: \n");
        printf("***********************************************\n");

        fgets(input, INPUT_BUFFER_SIZE, stdin);
        if (sscanf(input, "%d", &choice) != 1) {
            printf("Invalid input. Please enter a number between 1 and 4.\n");
            continue;
        }

        switch (choice) {
        case 1: // add garage
            do {
                printf("Enter Garage ID (5 digits): ");
                fgets(input, INPUT_BUFFER_SIZE, stdin);
                if (sscanf(input, "%d", &garage.id) != 1 || garage.id < 10000 || garage.id > 99999) {
                    printf("Invalid Garage ID. It must be a 5-digit number.\n");
                    continue;
                }
                break;
            } while (1);

            // Enter Garage Name
            do {
                printf("Enter Garage Name: ");
                fgets(garage.name, sizeof(garage.name), stdin);
                garage.name[strcspn(garage.name, "\n")] = 0;  // Remove newline character
                if (strlen(garage.name) == 0) {
                    printf("Garage name cannot be empty. Please enter a valid name.\n");
                }
                else {
                    break;
                }
            } while (1);

            // Enter Garage Address
            do {
                printf("Enter Garage Address: ");
                fgets(garage.address, sizeof(garage.address), stdin);
                garage.address[strcspn(garage.address, "\n")] = 0;  // Remove newline character
                if (strlen(garage.address) == 0) {
                    printf("Garage address cannot be empty. Please enter a valid address.\n");
                }
                else {
                    break;
                }
            } while (1);

            if (addGarage(storeManager->garageList, garage)) {
                printf("Garage added successfully.\n");
            }
            else {
                printf("Failed to add garage.\n");
            }
            break;
        case 2: // remove garage
            if (storeManager->garageList->head == NULL) {
                printf("No garages available to remove.\n");
            }
            else {
                printGarages(storeManager->garageList);

                int id;
                int validInput = 0; 

                do {
                    printf("Enter Garage ID to remove: ");
                    fgets(input, INPUT_BUFFER_SIZE, stdin);

					// check if the input is a number
                    validInput = 1; 
                    for (int i = 0; i < strlen(input) - 1; i++) { 
                        if (!isdigit(input[i])) {
							validInput = 0; // not a valid input
                            printf("Invalid ID format. ID must be a 5-digit number with no spaces or other characters.\n");
                            break;
                        }
                    }

                    // המרה למספר ובדיקה האם מספר בעל 5 ספרות
                    if (validInput && sscanf(input, "%d", &id) == 1 && id >= 10000 && id <= 99999) {
						break; // valid input
                    }
                    else {
                        printf("Invalid ID format. ID must be a 5-digit number.\n");
                    }

                } while (1); 

                if (removeGarage(storeManager->garageList, id)) {
                    printf("Garage removed successfully.\n");
                }
                else {
                    printf("Garage not found or failed to remove.\n");
                }
            }
            break;

        case 3: // print all garages
            printGarages(storeManager->garageList);
            break;
        case 4: // back to main menu
            printf("Returning to main menu...\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    } while (choice != 4);
}
