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
            addCarToCustomer(customer, manager->carInventory.cars[i]);
            removeCarFromInventoryWithoutFree(&manager->carInventory, serial_number);
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
            addPartToCustomer(customer, manager->partInventory.parts[i]);
            removePartFromInventoryWithoutFree(&manager->partInventory, serial_number);
            return 1;
        }
    }
    return 0;
}

void freeStoreManager(StoreManager* storeManager) {
    if (!storeManager) return;

    // free car inventory
    if (storeManager->carInventory.cars) {
        for (int i = 0; i < storeManager->carInventory.count; i++) {
            if (storeManager->carInventory.cars[i]) {
                free(storeManager->carInventory.cars[i]);
            }
        }
        free(storeManager->carInventory.cars);
    }

    // free part inventory
    if (storeManager->partInventory.parts) {
        for (int i = 0; i < storeManager->partInventory.count; i++) {
            if (storeManager->partInventory.parts[i]) {
                free(storeManager->partInventory.parts[i]->manufacturer);
                free(storeManager->partInventory.parts[i]);
            }
        }
        free(storeManager->partInventory.parts);
    }

    // free customer list
    if (storeManager->customerList.customers) {
        for (int i = 0; i < storeManager->customerList.count; i++) {
            Customer* customer = storeManager->customerList.customers[i];
            if (customer) {
                // free purchased parts
                if (customer->purchased_parts) {
                    LinkedListNode* currentPartNode = customer->purchased_parts->head;
                    while (currentPartNode) {
                        LinkedListNode* next = currentPartNode->next;
                        if (currentPartNode->data) {
                            free(currentPartNode->data); // free the part
                        }
                        free(currentPartNode);       // free the node
                        currentPartNode = next;
                    }
                    free(customer->purchased_parts);
                }

                // free purchased cars
                if (customer->purchased_cars) {
                    LinkedListNode* currentCarNode = customer->purchased_cars->head;
                    while (currentCarNode) {
                        LinkedListNode* next = currentCarNode->next;
                        free(currentCarNode); // free the node
                        currentCarNode = next;
                    }
                    free(customer->purchased_cars);
                }

                // free the customer
                free(customer);
            }
        }
        free(storeManager->customerList.customers);
    }

    // free garage list
    if (storeManager->garageList) {
        GarageNode* current = storeManager->garageList->head;
        while (current) {
            GarageNode* next = current->next;
            free(current);
            current = next;
        }
        free(storeManager->garageList);
    }

    // free the store manager
    free(storeManager);
}
