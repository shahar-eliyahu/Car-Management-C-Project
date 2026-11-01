#include "garage.h"
#include "constants.h" // הוספת קובץ הכותרת להדפסת שגיאות

GarageList* createGarageList() {
    GarageList* list = (GarageList*)malloc(sizeof(GarageList));
    if (list != NULL) {
        list->head = NULL;
    }
    else {
        PRINT_ERROR("Memory allocation failed for GarageList.");
    }
    return list;
}

int addGarage(GarageList* list, Garage garage) {
    if (list == NULL) {
        PRINT_ERROR("Garage list is not initialized.");
        return 0; // list is not initialized
    }

    // check if a garage with the same ID already exists
    GarageNode* current = list->head;
    while (current != NULL) {
        if (current->garage.id == garage.id) {
            PRINT_ERROR("A garage with the same ID already exists.");
            return 0; // already exists
        }
        current = current->next;
    }

    // allocate memory for the new garage node
    GarageNode* newNode = (GarageNode*)malloc(sizeof(GarageNode));
    if (newNode == NULL) {
        PRINT_ERROR("Memory allocation failed for new garage.");
        return 0; // failed
    }

    newNode->garage = garage;
    newNode->next = list->head;
    list->head = newNode;
    return 1; // success
}

void printGarages(const GarageList* list) {
    if (list == NULL) {
        PRINT_ERROR("Garage list not initialized.");
        return;
    }

    if (list->head == NULL) {
        printf("No garages in the list.\n");
        return;
    }

    GarageNode* current = list->head;
    while (current != NULL) {
        printf("Garage ID: %d, Name: %s, Address: %s\n",
            current->garage.id,
            current->garage.name,
            current->garage.address);
        current = current->next;
    }
}

// Appends at the end so a saved and reloaded list keeps its order
void appendGarageNode(GarageList* list, GarageNode* node) {
    if (!list || !node) return;
    node->next = NULL;
    if (!list->head) {
        list->head = node;
        return;
    }
    GarageNode* current = list->head;
    while (current->next) {
        current = current->next;
    }
    current->next = node;
}

void clearGarageList(GarageList* list) {
    if (!list) return;
    GarageNode* current = list->head;
    while (current) {
        GarageNode* next = current->next;
        free(current);
        current = next;
    }
    list->head = NULL;
}

int removeGarage(GarageList* list, int id) {
    if (list == NULL || list->head == NULL) {
        PRINT_ERROR("The list is empty or not initialized.");
        return 0; 
    }

    GarageNode* current = list->head;
    GarageNode* previous = NULL;

    while (current != NULL) {
        if (current->garage.id == id) {
            if (previous == NULL) { // the node to remove is the head
                list->head = current->next;
            }
            else {
                previous->next = current->next;
            }
            free(current);
            return 1; // success
        }
        previous = current;
        current = current->next;
    }

    PRINT_ERROR("Garage not found.");
    return 0; // not found
}
