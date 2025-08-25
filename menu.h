#ifndef MENU_H
#define MENU_H

#include "store_manager.h"
#include "constants.h"

// Function prototypes

void displayMenu(StoreManager* storeManager);
void handleLoadSystem(StoreManager* storeManager);
void handleSaveSystem(StoreManager* storeManager);
void handleAddMenu(StoreManager* storeManager);
void handleRemoveMenu(StoreManager* storeManager);
void handleSortMenu(StoreManager* storeManager);
void handleSearchMenu(StoreManager* storeManager);
void handleAddCustomer(StoreManager* storeManager);
void handlePurchaseMenu(StoreManager* storeManager);
void displayAllTransactions(const StoreManager* storeManager);
void handleIncreaseCarPrices(StoreManager* storeManager);
void handleManageGarageMenu(StoreManager* storeManager);

#endif// MENU_H