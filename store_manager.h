#ifndef STORE_MANAGER_H
#define STORE_MANAGER_H

#include "car.h"
#include "part.h"
#include "customer.h"
#include "garage.h"


typedef struct {
    CarInventory carInventory;
    PartInventory partInventory;
    CustomerList customerList;
    GarageList* garageList;
} StoreManager;

StoreManager* createStoreManager();
int addCar(StoreManager* manager, Car* car);
int addPart(StoreManager* manager, Part* part);
int addCustomer(StoreManager* manager, Customer* customer);
int removeCar(StoreManager* manager, int serial_number);
int removePart(StoreManager* manager, int serial_number);
int removeCustomer(StoreManager* manager, const char* id);
int purchaseCar(StoreManager* manager, Customer* customer, int serial_number);
int purchasePart(StoreManager* manager, Customer* customer, int serial_number);
void clearStore(StoreManager* manager);
void freeStoreManager(StoreManager* manager);

#endif // STORE_MANAGER_H
