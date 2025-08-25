#ifndef CAR_H
#define CAR_H

#include "constants.h"

typedef struct {
    char type[MAX_TYPE_LEN];
    char engine_type[MAX_ENGINE_TYPE_LEN];
    char brand[MAX_BRAND_LEN];
    char model[MAX_MODEL_LEN];
    float price;
    int serial_number;  // Changed to int
} Car;

typedef struct {
    Car** cars;  // Dynamic array of Car pointers
    int count;
} CarInventory;

Car* createCar(const char* type, const char* engine_type, const char* brand, const char* model, float price, CarInventory* inventory);
int addCarToInventory(CarInventory* inventory, Car* car);
int removeCarFromInventory(CarInventory* inventory, int serial_number);
void printCar(void* car);
void printAllCars(const CarInventory* inventory);
void sortCars(CarInventory* inventory, int (*compare)(const void*, const void*));

int compareByPrice(const void* a, const void* b);
int compareByBrand(const void* a, const void* b);
int compareBySerialNumber(const void* a, const void* b);

int searchByPrice(const void* key, const void* element);
int searchByBrand(const void* key, const void* element);
int searchBySerialNumber(const void* key, const void* element);
Car* findCar(CarInventory* inventory, const void* key, int (*compare)(const void*, const void*));

int removeCarFromInventoryWithoutFree(CarInventory* inventory, int serial_number);
void freeCarInventory(CarInventory* inventory);
void genericFunction(void** array, size_t count, void (*processFunc)(void*));
void freeSingleCar(void* car);
void increaseCarPrice(void* car);
void increaseAllCarPrices(CarInventory* inventory);
#endif // CAR_H
