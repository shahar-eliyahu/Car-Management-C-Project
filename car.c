#define _CRT_SECURE_NO_WARNINGS
#include "car.h"
#include "constants.h" // הוספת קובץ הכותרת להדפסת שגיאות
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function implementations
Car* createCar(const char* type, const char* engine_type, const char* brand, const char* model, float price, CarInventory* inventory) {
    static int serialCounter = 100000000;

    Car* car = (Car*)malloc(sizeof(Car));
    if (!car) {
        PRINT_ERROR("Failed to allocate memory for Car.");
        return NULL;
    }

    strcpy(car->type, type);
    strcpy(car->engine_type, engine_type);
    strcpy(car->brand, brand);
    strcpy(car->model, model);
    car->price = price;

    int isUnique = 0;
    while (!isUnique) {
        car->serial_number = serialCounter;
        isUnique = 1;

        for (int i = 0; i < inventory->count; i++) {
            if (inventory->cars[i]->serial_number == car->serial_number) {
                isUnique = 0;
                break;
            }
        }

        if (!isUnique) {
            serialCounter++;
        }
    }

    serialCounter++;
    return car;
}

int addCarToInventory(CarInventory* inventory, Car* car) {
    if (!inventory || !car) {
        PRINT_ERROR("Invalid input to addCarToInventory.");
        return 0;
    }

    Car** temp = (Car**)realloc(inventory->cars, (inventory->count + 1) * sizeof(Car*));
    if (temp) {
        inventory->cars = temp;
        inventory->cars[inventory->count++] = car;
        return 1; // Success
    }
    else {
        PRINT_ERROR("Failed to allocate memory for new car.");
        return 0; // Failure
    }
}

int removeCarFromInventory(CarInventory* inventory, int serial_number) {
    if (!inventory) return 0;

    for (size_t i = 0; i < inventory->count; i++) {
        if (inventory->cars[i]->serial_number == serial_number) {
            free(inventory->cars[i]);

            for (size_t j = i; j < inventory->count - 1; j++) {
                inventory->cars[j] = inventory->cars[j + 1];
            }
            inventory->count--;

            if (inventory->count > 0) {
                Car** temp = (Car**)realloc(inventory->cars, inventory->count * sizeof(Car*));
                if (temp) {
                    inventory->cars = temp;
                }
                else {
                    PRINT_ERROR("Failed to reallocate memory after removing a car.");
                }
            }
            else {
                free(inventory->cars);
                inventory->cars = NULL;
            }
            return 1; // Car found and removed
        }
    }
    return 0; // Car not found
}

int removeCarFromInventoryWithoutFree(CarInventory* inventory, int serial_number) {
    if (!inventory) return 0;

    for (size_t i = 0; i < inventory->count; i++) {
        if (inventory->cars[i]->serial_number == serial_number) {
            for (size_t j = i; j < inventory->count - 1; j++) {
                inventory->cars[j] = inventory->cars[j + 1];
            }
            inventory->count--;

            if (inventory->count > 0) {
                Car** temp = (Car**)realloc(inventory->cars, inventory->count * sizeof(Car*));
                if (temp) {
                    inventory->cars = temp;
                }
                else {
                    PRINT_ERROR("Failed to reallocate memory after removing a car.");
                }
            }
            else {
                free(inventory->cars);
                inventory->cars = NULL;
            }
            return 1;
        }
    }
    return 0;
}

int compareByPrice(const void* a, const void* b) {
    Car* carA = *(Car**)a;
    Car* carB = *(Car**)b;
    if (carA->price < carB->price) return -1;
    if (carA->price > carB->price) return 1;
    return 0;
}

int compareBySerialNumber(const void* a, const void* b) {
    Car* carA = *(Car**)a;
    Car* carB = *(Car**)b;
    return carA->serial_number - carB->serial_number;
}

int compareByBrand(const void* a, const void* b) {
    Car* carA = *(Car**)a;
    Car* carB = *(Car**)b;
    return strcmp(carA->brand, carB->brand);
}

void sortCars(CarInventory* inventory, int (*compare)(const void*, const void*)) {
    qsort(inventory->cars, inventory->count, sizeof(Car*), compare);
}

Car* findCar(CarInventory* inventory, const void* key, int (*compare)(const void*, const void*)) {
    Car** result = (Car**)bsearch(key, inventory->cars, inventory->count, sizeof(Car*), compare);
    if (result != NULL) {
        return *result;
    }
    else {
        return NULL;
    }
}

// searchByPrice function to be used with bsearch
int searchByPrice(const void* key, const void* element) {
    float* targetPrice = (float*)key;
    Car* car = *(Car**)element;
    if (*targetPrice < car->price) return -1;
    if (*targetPrice > car->price) return 1;
    return 0;
}

// searchBySerialNumber function to be used with bsearch
int searchBySerialNumber(const void* key, const void* element) {
    int* targetSerial = (int*)key;
    Car* car = *(Car**)element;
    return *targetSerial - car->serial_number;
}

// searchByBrand function to be used with bsearch
int searchByBrand(const void* key, const void* element) {
    char* targetBrand = (char*)key;
    Car* car = *(Car**)element;
    return strcmp(targetBrand, car->brand);
}

void freeSingleCar(void* car) {
    free((Car*)car);
}

void printCar(void* car) {
    if (!car) return;

    Car* carObj = (Car*)car;

    printf("+---------------------------------------------------------------+\n");
    printf("| Serial Number : %-45d |\n", carObj->serial_number);
    printf("| Type          : %-45s |\n", carObj->type);
    printf("| Engine Type   : %-45s |\n", carObj->engine_type);
    printf("| Brand         : %-45s |\n", carObj->brand);
    printf("| Model         : %-45s |\n", carObj->model);
    printf("| Price         : %-45.2f |\n", carObj->price);
    printf("+---------------------------------------------------------------+\n");
}

// Generic function to process an array of pointers
void genericFunction(void** array, size_t count, void (*processFunc)(void*)) {
    for (size_t i = 0; i < count; i++) {
        processFunc(array[i]);
    }
}

void printAllCars(const CarInventory* inventory) {
    if (inventory->count == 0) {
        printf("No cars available in the inventory.\n");
        return;
    }
    genericFunction((void**)inventory->cars, inventory->count, printCar);
}

void freeCarInventory(CarInventory* inventory) {
    genericFunction((void**)inventory->cars, inventory->count, freeSingleCar);
    free(inventory->cars);
    inventory->count = 0;
}

void increaseCarPrice(void* car) {
    Car* c = (Car*)car;
    c->price += c->price * (PERCENTAGE_INCREASE_CAR_PRICES / 100.0f);
}

void increaseAllCarPrices(CarInventory* inventory) {
    // Call genericFunction with the adjusted function signature
    genericFunction((void**)inventory->cars, inventory->count, increaseCarPrice);
}
