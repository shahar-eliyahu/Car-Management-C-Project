#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "store_manager.h"
#include "menu.h"
#include "dashboard.h"
#include "file_operations.h"

// Previously the program always saved to system_data.txt on exit, which wiped the data file
// whenever the user exited without loading it first. Now the user decides.
static void askToSaveOnExit(StoreManager* storeManager) {
    char input[INPUT_BUFFER_SIZE];
    printf("Save the current data to system_data.txt before exiting? (1 = Yes, 0 = No): ");
    if (fgets(input, sizeof(input), stdin) && input[0] == '1') {
        saveToFile("system_data.txt", storeManager, 0);
    }
}

int main(int argc, char* argv[]) {
    StoreManager* storeManager = createStoreManager();
    if (!storeManager) {
        return 1;
    }

    // The dashboard is the default interface; --classic runs the original text menu
    if (argc > 1 && strcmp(argv[1], "--classic") == 0) {
        displayMenu(storeManager);
    }
    else {
        runDashboard(storeManager);
    }

    askToSaveOnExit(storeManager);
    freeStoreManager(storeManager);
    return 0;
}
