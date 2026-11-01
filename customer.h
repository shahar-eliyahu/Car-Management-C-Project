#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "constants.h"
#include "car.h"
#include "part.h"

// Linked list node
typedef struct LinkedListNode {
    void* data;
    struct LinkedListNode* next;
    struct LinkedListNode* prev;
} LinkedListNode;

// Linked list
typedef struct {
    LinkedListNode* head;
    LinkedListNode* tail;
    int size;
} LinkedList;

// Customer structure
typedef struct {
    char first_name[MAX_CUSTOMER_NAME_LEN];
    char last_name[MAX_CUSTOMER_NAME_LEN];
    char id[MAX_CUSTOMER_ID_LEN + 1]; // 9 digits + '\0'
    LinkedList* purchased_cars;  // Linked list of Car*
    LinkedList* purchased_parts; // Linked list of Part*
} Customer;

typedef struct {
    Customer** customers;  // Dynamic array of Customer pointers
    int count;
} CustomerList;

// Function prototypes
Customer* createCustomer(const char* first_name, const char* last_name, const char* id);
int addCustomerToList(CustomerList* list, Customer* customer);
int removeCustomerFromList(CustomerList* list, const char* id);
void printCustomer(const Customer* customer);
void printAllCustomers(const CustomerList* list);
int addCarToCustomer(Customer* customer, Car* car);
int addPartToCustomer(Customer* customer, Part* part);
void freeCustomerPurchases(Customer* customer);
void freeCustomerList(CustomerList* list);

#endif// CUSTOMER_H
