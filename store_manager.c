#define _CRT_SECURE_NO_WARNINGS
#include "store_manager.h"
#include <stdlib.h>

// Function implementations

StoreManager* createStoreManager() {
    StoreManager* manager = (StoreManager*)malloc(sizeof(StoreManager));
    if (!manager) {
        PRINT_ERROR("Failed to allocate memory for StoreManager.");
        return NULL;
    }

    // initialize car inventory
    manager->carInventory.cars = NULL;
    manager->carInventory.count = 0;

    // initialize part inventory
    manager->partInventory.parts = NULL;
    manager->partInventory.count = 0;

    // initialize customer list
    manager->customerList.customers = NULL;
    manager->customerList.count = 0;

    // initialize garage list
    manager->garageList = createGarageList();
    if (!manager->garageList) {
        PRINT_ERROR("Failed to create garage list.");
        free(manager);
        return NULL;
    }

    return manager;
}

int addCar(StoreManager* manager, Car* car) {
    if (!manager || !car) return 0;
    return addCarToInventory(&manager->carInventory, car);
}

int addPart(StoreManager* manager, Part* part) {
    if (!manager || !part) return 0;
    return addPartToInventory(&manager->partInventory, part);
}

int addCustomer(StoreManager* storeManager, Customer* customer) {
    if (!storeManager || !customer) return 0;

    storeManager->customerList.customers = realloc(storeManager->customerList.customers, (storeManager->customerList.count + 1) * sizeof(Customer*));
    if (storeManager->customerList.customers == NULL) {
        PRINT_ERROR("Memory allocation failed while adding a customer.");
        return 0;
    }

    storeManager->customerList.customers[storeManager->customerList.count] = customer;
    storeManager->customerList.count++;
    return 1;
}

int removeCar(StoreManager* manager, int serial_number) {
    if (!manager) return 0;
    return removeCarFromInventory(&manager->carInventory, serial_number);
}

int removePart(StoreManager* manager, int serial_number) {
    if (!manager) return 0;
    return removePartFromInventory(&manager->partInventory, serial_number);
}

int removeCustomer(StoreManager* manager, const char* id) {
    if (!manager || !id) return 0;
    removeCustomerFromList(&manager->customerList, id);
    return 1; // Assuming customer removal always succeeds if found
}

int purchaseCar(StoreManager* manager, Customer* customer, int serial_number) {
    if (!manager || !customer) {
        return 0;
    }
    for (int i = 0; i < manager->carInventory.count; i++) {
        if (manager->carInventory.cars[i]->serial_number == serial_number) {
            if (!addCarToCustomer(customer, manager->carInventory.cars[i])) {
                return 0;
            }
            removeCarFromInventory(&manager->carInventory, serial_number);
            return 1;
        }
    }
    return 0;
}

int purchasePart(StoreManager* manager, Customer* customer, int serial_number) {
    if (!manager || !customer) {
        return 0;
    }
    for (int i = 0; i < manager->partInventory.count; i++) {
        if (manager->partInventory.parts[i]->serial_number == serial_number) {
            if (!addPartToCustomer(customer, manager->partInventory.parts[i])) {
                return 0;
            }
            removePartFromInventory(&manager->partInventory, serial_number);
            return 1;
        }
    }
    return 0;
}

// Frees every car, part, customer and garage, leaving an empty store that can be reused
void clearStore(StoreManager* storeManager) {
    if (!storeManager) return;

    for (int i = 0; i < storeManager->carInventory.count; i++) {
        free(storeManager->carInventory.cars[i]);
    }
    free(storeManager->carInventory.cars);
    storeManager->carInventory.cars = NULL;
    storeManager->carInventory.count = 0;

    for (int i = 0; i < storeManager->partInventory.count; i++) {
        freePart(storeManager->partInventory.parts[i]);
    }
    free(storeManager->partInventory.parts);
    storeManager->partInventory.parts = NULL;
    storeManager->partInventory.count = 0;

    // also frees each customer's purchased cars and parts
    freeCustomerList(&storeManager->customerList);

    clearGarageList(storeManager->garageList);
}

void freeStoreManager(StoreManager* storeManager) {
    if (!storeManager) return;
    clearStore(storeManager);
    free(storeManager->garageList);
    free(storeManager);
}
