#ifndef PART_H
#define PART_H


#include "constants.h"

typedef struct {
    float length;
    float width;
    float height;
} Dimensions;

typedef struct {
    int serial_number;  // Changed to int
    char name[MAX_NAME_LEN];
    char brand[MAX_BRAND_LEN];
    float price;
    char* manufacturer;
    Dimensions dimensions;
} Part;

typedef struct {
    Part** parts;  // Dynamic array of Part pointers
    int count;
} PartInventory;

Part* createPart(const char* name, const char* brand, float price, const char* manufacturer, float length, float width, float height, PartInventory* inventory);
int addPartToInventory(PartInventory* inventory, Part* part);
int removePartFromInventory(PartInventory* inventory, int serial_number);
void printPart(const Part* part);
void printAllParts(const PartInventory* inventory);
void sortParts(PartInventory* inventory, int (*compare)(const void*, const void*));
Part* searchPart(const PartInventory* inventory, int serial_number);
int comparePartsByPrice(const void* a, const void* b);
int comparePartsByBrand(const void* a, const void* b);
Part* searchPartByBrand(const PartInventory* inventory, const char* brand);
int removePartFromInventoryWithoutFree(PartInventory* inventory, int serial_number);

void freePartInventory(PartInventory* inventory);
char* duplicateString(const char* source);
Part* copyPart(const Part* part);
void freePart(Part* part);

#endif// PART_H
