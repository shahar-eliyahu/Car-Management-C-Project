#define _CRT_SECURE_NO_WARNINGS
#include "customer.h"
#include "constants.h" // הוספת קובץ הכותרת להדפסת שגיאות
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function implementations

Customer* createCustomer(const char* first_name, const char* last_name, const char* id) {
    Customer* customer = (Customer*)malloc(sizeof(Customer));
    if (!customer) {
        PRINT_ERROR("Failed to allocate memory for Customer.");
        return NULL;
    }

    strcpy(customer->first_name, first_name);
    strcpy(customer->last_name, last_name);
    strcpy(customer->id, id);

    customer->purchased_cars = (LinkedList*)malloc(sizeof(LinkedList));
    if (!customer->purchased_cars) {
        PRINT_ERROR("Failed to allocate memory for purchased cars list.");
        free(customer);
        return NULL;
    }

    customer->purchased_parts = (LinkedList*)malloc(sizeof(LinkedList));
    if (!customer->purchased_parts) {
        PRINT_ERROR("Failed to allocate memory for purchased parts list.");
        free(customer->purchased_cars);
        free(customer);
        return NULL;
    }

    customer->purchased_cars->head = customer->purchased_cars->tail = NULL;
    customer->purchased_parts->head = customer->purchased_parts->tail = NULL;
    customer->purchased_cars->size = customer->purchased_parts->size = 0;

    return customer;
}

int addCustomerToList(CustomerList* list, Customer* customer) {
    if (!list || !customer) {
        PRINT_ERROR("Invalid input to addCustomerToList.");
        return 0;
    }

    Customer** temp = (Customer**)realloc(list->customers, (list->count + 1) * sizeof(Customer*));
    if (temp) {
        list->customers = temp;
        list->customers[list->count++] = customer;
        return 1;  // Success
    }
    else {
        PRINT_ERROR("Failed to allocate memory for new customer.");
        return 0;  // Failure
    }
}

int removeCustomerFromList(CustomerList* list, const char* id) {
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->customers[i]->id, id) == 0) {
            LinkedListNode* current = list->customers[i]->purchased_cars->head;
            while (current) {
                LinkedListNode* next = current->next;
                free(current->data);
                free(current);
                current = next;
            }

            current = list->customers[i]->purchased_parts->head;
            while (current) {
                LinkedListNode* next = current->next;
                free(current->data);
                free(current);
                current = next;
            }

            free(list->customers[i]->purchased_cars);
            free(list->customers[i]->purchased_parts);
            free(list->customers[i]);

            for (size_t j = i; j < list->count - 1; j++) {
                list->customers[j] = list->customers[j + 1];
            }

            list->count--;

            Customer** temp = (Customer**)realloc(list->customers, list->count * sizeof(Customer*));
            if (temp || list->count == 0) {
                list->customers = temp;
            }
            else {
                PRINT_ERROR("Failed to shrink customer list.");
                return 0;  // Failure
            }
            return 1;  // Success
        }
    }
    return 0;  // Customer not found
}

void printCustomer(const Customer* customer) {
    if (!customer) {
        PRINT_ERROR("Customer data is NULL.");
        return;
    }
    printf("Customer ID: %s ,", customer->id);
    printf("Name: %s %s\n", customer->first_name, customer->last_name);
}

void printAllCustomers(const CustomerList* list) {
    if (!list || list->count == 0) {
        PRINT_ERROR("No customers to display.");
        return;
    }

    for (int i = 0; i < list->count; i++) {
        printCustomer(list->customers[i]);
    }
}

int addCarToCustomer(Customer* customer, Car* car) {
    if (!customer || !car) {
        PRINT_ERROR("Invalid input to addCarToCustomer.");
        return 0;  // Failure
    }

    Car* carCopy = (Car*)malloc(sizeof(Car));
    if (!carCopy) {
        PRINT_ERROR("Memory allocation failed for car copy.");
        return 0;  // Failure
    }
    *carCopy = *car;

    LinkedListNode* newNode = (LinkedListNode*)malloc(sizeof(LinkedListNode));
    if (!newNode) {
        PRINT_ERROR("Memory allocation failed for linked list node.");
        free(carCopy);
        return 0;  // Failure
    }

    newNode->data = carCopy;
    newNode->next = NULL;
    newNode->prev = NULL;

    if (customer->purchased_cars->head == NULL) {
        customer->purchased_cars->head = newNode;
        customer->purchased_cars->tail = newNode;
    }
    else {
        customer->purchased_cars->tail->next = newNode;
        newNode->prev = customer->purchased_cars->tail;
        customer->purchased_cars->tail = newNode;
    }

    customer->purchased_cars->size++;
    return 1;  // Success
}

int addPartToCustomer(Customer* customer, Part* part) {
    if (!customer || !part) {
        PRINT_ERROR("Invalid input to addPartToCustomer.");
        return 0;  // Failure
    }

    Part* partCopy = (Part*)malloc(sizeof(Part));
    if (!partCopy) {
        PRINT_ERROR("Memory allocation failed for part copy.");
        return 0;  // Failure
    }
    *partCopy = *part;

    LinkedListNode* node = (LinkedListNode*)malloc(sizeof(LinkedListNode));
    if (!node) {
        PRINT_ERROR("Memory allocation failed for linked list node.");
        free(partCopy);
        return 0;  // Failure
    }

    node->data = partCopy;
    node->next = node->prev = NULL;

    if (!customer->purchased_parts->head) {
        customer->purchased_parts->head = customer->purchased_parts->tail = node;
    }
    else {
        customer->purchased_parts->tail->next = node;
        node->prev = customer->purchased_parts->tail;
        customer->purchased_parts->tail = node;
    }
    customer->purchased_parts->size++;
    return 1;  // Success
}

void freeCustomerList(CustomerList* list) {
    for (size_t i = 0; i < list->count; i++) {
        LinkedListNode* current = list->customers[i]->purchased_cars->head;
        while (current) {
            LinkedListNode* next = current->next;
            free(current->data);
            free(current);
            current = next;
        }

        current = list->customers[i]->purchased_parts->head;
        while (current) {
            LinkedListNode* next = current->next;
            free(current->data);
            free(current);
            current = next;
        }

        free(list->customers[i]->purchased_cars);
        free(list->customers[i]->purchased_parts);
        free(list->customers[i]);
    }
    free(list->customers);
    list->count = 0;
}
