#define _CRT_SECURE_NO_WARNINGS
#include "store_manager.h"
#include "menu.h"
#include "file_operations.h"

int main() {
    StoreManager* storeManager = createStoreManager();
    displayMenu(storeManager);
    saveToFile("system_data.txt", storeManager, 0);
    freeStoreManager(storeManager);
    return 0;
}