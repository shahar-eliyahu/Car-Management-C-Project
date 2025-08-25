#define _CRT_SECURE_NO_WARNINGS
#include "part.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Part* createPart(const char* name, const char* brand, float price, const char* manufacturer, float length, float width, float height, PartInventory* inventory) {
    static int serialCounter = 100000000;

    Part* part = (Part*)malloc(sizeof(Part));
    if (!part) {
        PRINT_ERROR("Failed to allocate memory for Part.");
        return NULL;
    }

    strcpy(part->name, name);
    strcpy(part->brand, brand);
    part->price = price;

    part->manufacturer = (char*)malloc(strlen(manufacturer) + 1);
    if (!part->manufacturer) {
        PRINT_ERROR("Failed to allocate memory for Manufacturer.");
        free(part);
        return NULL;
    }
    strcpy(part->manufacturer, manufacturer);

    part->dimensions.length = length;
    part->dimensions.width = width;
    part->dimensions.height = height;

    int isUnique = 0;
    while (!isUnique) {
        part->serial_number = serialCounter;
        isUnique = 1;

        for (int i = 0; i < inventory->count; i++) {
            if (inventory->parts[i]->serial_number == part->serial_number) {
                isUnique = 0;
                break;
            }
        }

        if (!isUnique) {
            serialCounter++;
        }
    }

    serialCounter++;
    return part;
}

int addPartToInventory(PartInventory* inventory, Part* part) {
    if (!inventory || !part) {
        PRINT_ERROR("Invalid input to addPartToInventory.");
        return 0;
    }

    Part** newParts = (Part**)realloc(inventory->parts, (inventory->count + 1) * sizeof(Part*));
    if (!newParts) {
        PRINT_ERROR("Memory allocation failed in addPartToInventory.");
        return 0;
    }

    inventory->parts = newParts;
    inventory->parts[inventory->count++] = part;
    return 1;
}

int removePartFromInventory(PartInventory* inventory, int serial_number) {
    if (!inventory) return 0;

    for (size_t i = 0; i < inventory->count; i++) {
        if (inventory->parts[i]->serial_number == serial_number) {
            free(inventory->parts[i]->manufacturer);
            free(inventory->parts[i]);
            for (size_t j = i; j < inventory->count - 1; j++) {
                inventory->parts[j] = inventory->parts[j + 1];
            }
            inventory->count--;

            if (inventory->count > 0) {
                Part** temp = (Part**)realloc(inventory->parts, inventory->count * sizeof(Part*));
                if (temp) {
                    inventory->parts = temp;
                }
                else {
                    PRINT_ERROR("Failed to reallocate memory after removing a part.");
                }
            }
            else {
                free(inventory->parts);
                inventory->parts = NULL;
            }
            return 1;
        }
    }
    return 0;
}

void printPart(const Part* part) {
    if (!part) return;
    printf("+-------------------------------------------------+\n");
    printf("| Serial Number  : %-30d |\n", part->serial_number);
    printf("| Name           : %-30s |\n", part->name);
    printf("| Brand          : %-30s |\n", part->brand);
    printf("| Price          : %-30.2f |\n", part->price);
    printf("| Manufacturer   : %-30s |\n", part->manufacturer);
    printf("| Dimensions     : Length = %-8.2f Width = %-8.2f Height = %-8.2f |\n",
        part->dimensions.length, part->dimensions.width, part->dimensions.height);
    printf("+-------------------------------------------------+\n");
}

void printAllParts(const PartInventory* inventory) {
    if (inventory->count == 0) {
        printf("No parts available in the inventory.\n");
        return;
    }

    for (size_t i = 0; i < inventory->count; i++) {
        printPart(inventory->parts[i]);
    }
}

int comparePartsByPrice(const void* a, const void* b) {
    Part* partA = *(Part**)a;
    Part* partB = *(Part**)b;
    return (partA->price > partB->price) - (partA->price < partB->price);
}

int comparePartsByBrand(const void* a, const void* b) {
    Part* partA = *(Part**)a;
    Part* partB = *(Part**)b;
    return strcmp(partA->brand, partB->brand);
}

void sortParts(PartInventory* inventory, int (*compare)(const void*, const void*)) {
    qsort(inventory->parts, inventory->count, sizeof(Part*), compare);
}

Part* searchPart(const PartInventory* inventory, int serial_number) {
    for (size_t i = 0; i < inventory->count; i++) {
        if (inventory->parts[i]->serial_number == serial_number) {
            return inventory->parts[i];
        }
    }
    return NULL;
}

void freePartInventory(PartInventory* inventory) {
    for (size_t i = 0; i < inventory->count; i++) {
        free(inventory->parts[i]->manufacturer);
        free(inventory->parts[i]);
    }
    free(inventory->parts);
    inventory->count = 0;
}

int removePartFromInventoryWithoutFree(PartInventory* inventory, int serial_number) {
    if (!inventory) return 0;

    for (size_t i = 0; i < inventory->count; i++) {
        if (inventory->parts[i]->serial_number == serial_number) {
            // Remove the part from the inventory
            for (size_t j = i; j < inventory->count - 1; j++) {
                inventory->parts[j] = inventory->parts[j + 1];
            }
            inventory->count--;

            if (inventory->count > 0) {
                Part** temp = (Part**)realloc(inventory->parts, inventory->count * sizeof(Part*));
                if (temp) {
                    inventory->parts = temp;
                }
                else {
                    PRINT_ERROR("Failed to reallocate memory after removing a part.");
                }
            }
            else {
                free(inventory->parts);
                inventory->parts = NULL;
            }
            return 1;
        }
    }
    return 0;
}

Part* searchPartByBrand(const PartInventory* inventory, const char* brand) {
    if (!inventory || !brand) {
        return NULL; // Return NULL if inventory or brand is NULL
    }

    for (size_t i = 0; i < inventory->count; i++) {
        if (strcmp(inventory->parts[i]->brand, brand) == 0) {
            return inventory->parts[i]; // Return the first part that matches the brand
        }
    }

    return NULL; // Return NULL if no part with the specified brand is found
}
