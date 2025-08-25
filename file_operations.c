#define _CRT_SECURE_NO_WARNINGS
#include "file_operations.h"
#include "constants.h" // Include the header file for error macros
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to save a Part struct in a compressed format
void saveCompressedPartToFile(FILE* file, const Part* part) {
    if (!file || !part) return;

    // Write basic fields without padding
    fwrite(&part->serial_number, sizeof(int), 1, file);
    fwrite(part->name, sizeof(char), MAX_NAME_LEN, file);
    fwrite(part->brand, sizeof(char), MAX_BRAND_LEN, file);
    fwrite(&part->price, sizeof(float), 1, file);

    // Write manufacturer dynamically
    size_t len = strlen(part->manufacturer) + 1;
    fwrite(&len, sizeof(size_t), 1, file);
    fwrite(part->manufacturer, sizeof(char), len, file);

    // Write dimensions without padding
    fwrite(&part->dimensions.length, sizeof(float), 1, file);
    fwrite(&part->dimensions.width, sizeof(float), 1, file);
    fwrite(&part->dimensions.height, sizeof(float), 1, file);
}

// Function to load a Part struct from a compressed format
Part* loadCompressedPartFromFile(FILE* file) {
    if (!file) return NULL;

    Part* part = (Part*)malloc(sizeof(Part));
    if (!part) {
        PRINT_ERROR("Memory allocation failed for part.");
        return NULL;
    }

    // Read basic fields
    fread(&part->serial_number, sizeof(int), 1, file);
    fread(part->name, sizeof(char), MAX_NAME_LEN, file);
    fread(part->brand, sizeof(char), MAX_BRAND_LEN, file);
    fread(&part->price, sizeof(float), 1, file);

    // Read manufacturer dynamically
    size_t len;
    fread(&len, sizeof(size_t), 1, file);
    part->manufacturer = (char*)malloc(len);
    if (!part->manufacturer) {
        PRINT_ERROR("Memory allocation failed for part manufacturer.");
        free(part);
        return NULL;
    }
    fread(part->manufacturer, sizeof(char), len, file);

    // Read dimensions
    fread(&part->dimensions.length, sizeof(float), 1, file);
    fread(&part->dimensions.width, sizeof(float), 1, file);
    fread(&part->dimensions.height, sizeof(float), 1, file);

    return part;
}

int saveToFile(const char* filename, const StoreManager* store, int isBinary) {
    FILE* file = fopen(filename, isBinary ? "wb" : "w");
    if (!file) {
        PRINT_ERROR("Unable to open file for writing.");
        return 0;
    }

    if (isBinary) {
        // Save car inventory in binary format
        if (fwrite(&store->carInventory.count, sizeof(size_t), 1, file) != 1) {
            PRINT_ERROR("Error writing car inventory count to file.");
            fclose(file);
            return 0;
        }
        for (size_t i = 0; i < store->carInventory.count; i++) {
            if (fwrite(store->carInventory.cars[i], sizeof(Car), 1, file) != 1) {
                PRINT_ERROR("Error writing car data to file.");
                fclose(file);
                return 0;
            }
        }

        // Save part inventory in compressed binary format
        if (fwrite(&store->partInventory.count, sizeof(size_t), 1, file) != 1) {
            PRINT_ERROR("Error writing part inventory count to file.");
            fclose(file);
            return 0;
        }
        for (size_t i = 0; i < store->partInventory.count; i++) {
            saveCompressedPartToFile(file, store->partInventory.parts[i]);
        }

        // Save customer list in binary format
        if (fwrite(&store->customerList.count, sizeof(size_t), 1, file) != 1) {
            PRINT_ERROR("Error writing customer list count to file.");
            fclose(file);
            return 0;
        }
        for (size_t i = 0; i < store->customerList.count; i++) {
            Customer* customer = store->customerList.customers[i];
            if (fwrite(customer, sizeof(Customer), 1, file) != 1) {
                PRINT_ERROR("Error writing customer data to file.");
                fclose(file);
                return 0;
            }

            // Save customer's car purchases
            size_t car_count = customer->purchased_cars->size;
            if (fwrite(&car_count, sizeof(size_t), 1, file) != 1) {
                PRINT_ERROR("Error writing car purchase count to file.");
                fclose(file);
                return 0;
            }
            LinkedListNode* current = customer->purchased_cars->head;
            while (current) {
                Car* car = (Car*)current->data;
                if (fwrite(car, sizeof(Car), 1, file) != 1) {
                    PRINT_ERROR("Error writing purchased car data to file.");
                    fclose(file);
                    return 0;
                }
                current = current->next;
            }

            // Save customer's part purchases
            size_t part_count = customer->purchased_parts->size;
            if (fwrite(&part_count, sizeof(size_t), 1, file) != 1) {
                PRINT_ERROR("Error writing part purchase count to file.");
                fclose(file);
                return 0;
            }
            current = customer->purchased_parts->head;
            while (current) {
                Part* part = (Part*)current->data;
                saveCompressedPartToFile(file, part);
                current = current->next;
            }
        }

        // Save garage list in binary format
        GarageNode* currentGarage = store->garageList->head;
        size_t garageCount = 0;

        // Count the number of garages
        while (currentGarage) {
            garageCount++;
            currentGarage = currentGarage->next;
        }

        // Save the number of garages
        if (fwrite(&garageCount, sizeof(size_t), 1, file) != 1) {
            PRINT_ERROR("Error writing garage count to file.");
            fclose(file);
            return 0;
        }

        // Save garage data
        currentGarage = store->garageList->head;
        while (currentGarage) {
            if (fwrite(&currentGarage->garage, sizeof(Garage), 1, file) != 1) {
                PRINT_ERROR("Error writing garage data to file.");
                fclose(file);
                return 0;
            }
            currentGarage = currentGarage->next;
        }
    }
    else {
        // Save car inventory in text format
        if (fprintf(file, "%zu\n", store->carInventory.count) < 0) {
            PRINT_ERROR("Error writing car inventory count to file.");
            fclose(file);
            return 0;
        }
        for (size_t i = 0; i < store->carInventory.count; i++) {
            Car* car = store->carInventory.cars[i];
            if (fprintf(file, "%s,%s,%s,%s,%.2f,%d\n",
                car->type, car->engine_type, car->brand, car->model, car->price, car->serial_number) < 0) {
                PRINT_ERROR("Error writing car data to file.");
                fclose(file);
                return 0;
            }
        }

        // Save part inventory in text format
        if (fprintf(file, "%zu\n", store->partInventory.count) < 0) {
            PRINT_ERROR("Error writing part inventory count to file.");
            fclose(file);
            return 0;
        }
        for (size_t i = 0; i < store->partInventory.count; i++) {
            Part* part = store->partInventory.parts[i];
            if (fprintf(file, "%d,%s,%s,%.2f,%s,%.2f,%.2f,%.2f\n",
                part->serial_number, part->name, part->brand, part->price, part->manufacturer,
                part->dimensions.length, part->dimensions.width, part->dimensions.height) < 0) {
                PRINT_ERROR("Error writing part data to file.");
                fclose(file);
                return 0;
            }
        }

        // Save customer list in text format
        if (fprintf(file, "%zu\n", store->customerList.count) < 0) {
            PRINT_ERROR("Error writing customer list count to file.");
            fclose(file);
            return 0;
        }
        for (size_t i = 0; i < store->customerList.count; i++) {
            Customer* customer = store->customerList.customers[i];
            if (fprintf(file, "%s,%s,%s\n", customer->first_name, customer->last_name, customer->id) < 0) {
                PRINT_ERROR("Error writing customer data to file.");
                fclose(file);
                return 0;
            }

            // Save customer's car purchases
            if (fprintf(file, "%zu\n", customer->purchased_cars->size) < 0) {
                PRINT_ERROR("Error writing car purchase count to file.");
                fclose(file);
                return 0;
            }
            LinkedListNode* current = customer->purchased_cars->head;
            while (current) {
                Car* car = (Car*)current->data;
                if (fprintf(file, "%s,%s,%s,%s,%.2f,%d\n",
                    car->type, car->engine_type, car->brand, car->model, car->price, car->serial_number) < 0) {
                    PRINT_ERROR("Error writing purchased car data to file.");
                    fclose(file);
                    return 0;
                }
                current = current->next;
            }

            // Save customer's part purchases
            if (fprintf(file, "%zu\n", customer->purchased_parts->size) < 0) {
                PRINT_ERROR("Error writing part purchase count to file.");
                fclose(file);
                return 0;
            }
            current = customer->purchased_parts->head;
            while (current) {
                Part* part = (Part*)current->data;
                if (fprintf(file, "%d,%s,%s,%.2f,%s,%.2f,%.2f,%.2f\n",
                    part->serial_number, part->name, part->brand, part->price, part->manufacturer,
                    part->dimensions.length, part->dimensions.width, part->dimensions.height) < 0) {
                    PRINT_ERROR("Error writing purchased part data to file.");
                    fclose(file);
                    return 0;
                }
                current = current->next;
            }
        }

        // Save garage list in text format
        GarageNode* currentGarage = store->garageList->head;
        size_t garageCount = 0;

        // Count the number of garages
        while (currentGarage) {
            garageCount++;
            currentGarage = currentGarage->next;
        }

        // Save the number of garages
        if (fprintf(file, "%zu\n", garageCount) < 0) {
            PRINT_ERROR("Error writing garage count to file.");
            fclose(file);
            return 0;
        }

        // Save garage data
        currentGarage = store->garageList->head;
        while (currentGarage) {
            if (fprintf(file, "%d,%s,%s\n", currentGarage->garage.id, currentGarage->garage.name, currentGarage->garage.address) < 0) {
                PRINT_ERROR("Error writing garage data to file.");
                fclose(file);
                return 0;
            }
            currentGarage = currentGarage->next;
        }
    }

    fclose(file);
    printf("Data successfully saved to %s.\n", filename);
    return 1;
}

int loadFromFile(const char* filename, StoreManager* store, int isBinary) {
    FILE* file = fopen(filename, isBinary ? "rb" : "r");
    if (!file) {
        PRINT_ERROR("Error opening file.");
        return 0;
    }

    if (isBinary) {
        // Load car inventory in binary format
        if (fread(&store->carInventory.count, sizeof(size_t), 1, file) != 1) {
            PRINT_ERROR("Error reading car inventory count from file.");
            fclose(file);
            return 0;
        }
        if (store->carInventory.count > 0) {
            store->carInventory.cars = (Car**)malloc(store->carInventory.count * sizeof(Car*));
            if (!store->carInventory.cars) {
                PRINT_ERROR("Memory allocation failed for car inventory.");
                fclose(file);
                return 0;
            }
            for (size_t i = 0; i < store->carInventory.count; i++) {
                store->carInventory.cars[i] = (Car*)malloc(sizeof(Car));
                if (!store->carInventory.cars[i]) {
                    PRINT_ERROR("Memory allocation failed for car.");
                    fclose(file);
                    return 0;
                }
                if (fread(store->carInventory.cars[i], sizeof(Car), 1, file) != 1) {
                    PRINT_ERROR("Error reading car data from file.");
                    fclose(file);
                    return 0;
                }
            }
        }

        // Load part inventory in compressed binary format
        if (fread(&store->partInventory.count, sizeof(size_t), 1, file) != 1) {
            PRINT_ERROR("Error reading part inventory count from file.");
            fclose(file);
            return 0;
        }
        if (store->partInventory.count > 0) {
            store->partInventory.parts = (Part**)malloc(store->partInventory.count * sizeof(Part*));
            if (!store->partInventory.parts) {
                PRINT_ERROR("Memory allocation failed for part inventory.");
                fclose(file);
                return 0;
            }
            for (size_t i = 0; i < store->partInventory.count; i++) {
                store->partInventory.parts[i] = loadCompressedPartFromFile(file);
                if (!store->partInventory.parts[i]) {
                    PRINT_ERROR("Error reading compressed part data from file.");
                    fclose(file);
                    return 0;
                }
            }
        }

        // Load customer list in binary format
        if (fread(&store->customerList.count, sizeof(size_t), 1, file) != 1) {
            PRINT_ERROR("Error reading customer list count from file.");
            fclose(file);
            return 0;
        }
        if (store->customerList.count > 0) {
            store->customerList.customers = (Customer**)malloc(store->customerList.count * sizeof(Customer*));
            if (!store->customerList.customers) {
                PRINT_ERROR("Memory allocation failed for customer list.");
                fclose(file);
                return 0;
            }
            for (size_t i = 0; i < store->customerList.count; i++) {
                store->customerList.customers[i] = (Customer*)malloc(sizeof(Customer));
                if (!store->customerList.customers[i]) {
                    PRINT_ERROR("Memory allocation failed for customer.");
                    fclose(file);
                    return 0;
                }
                if (fread(store->customerList.customers[i], sizeof(Customer), 1, file) != 1) {
                    PRINT_ERROR("Error reading customer data from file.");
                    fclose(file);
                    return 0;
                }

                // Initialize linked lists for purchases
                store->customerList.customers[i]->purchased_cars = (LinkedList*)malloc(sizeof(LinkedList));
                store->customerList.customers[i]->purchased_parts = (LinkedList*)malloc(sizeof(LinkedList));
                store->customerList.customers[i]->purchased_cars->head = store->customerList.customers[i]->purchased_cars->tail = NULL;
                store->customerList.customers[i]->purchased_parts->head = store->customerList.customers[i]->purchased_parts->tail = NULL;
                store->customerList.customers[i]->purchased_cars->size = store->customerList.customers[i]->purchased_parts->size = 0;

                // Load purchased cars
                size_t car_count;
                if (fread(&car_count, sizeof(size_t), 1, file) != 1) {
                    PRINT_ERROR("Error reading car purchase count from file.");
                    fclose(file);
                    return 0;
                }
                for (size_t j = 0; j < car_count; j++) {
                    Car* car = (Car*)malloc(sizeof(Car));
                    if (!car) {
                        PRINT_ERROR("Memory allocation failed for purchased car.");
                        fclose(file);
                        return 0;
                    }
                    if (fread(car, sizeof(Car), 1, file) != 1) {
                        PRINT_ERROR("Error reading purchased car data from file.");
                        free(car);
                        fclose(file);
                        return 0;
                    }
                    LinkedListNode* node = (LinkedListNode*)malloc(sizeof(LinkedListNode));
                    if (!node) {
                        PRINT_ERROR("Memory allocation failed for linked list node.");
                        free(car);
                        fclose(file);
                        return 0;
                    }
                    node->data = car;
                    node->next = node->prev = NULL;
                    if (!store->customerList.customers[i]->purchased_cars->head) {
                        store->customerList.customers[i]->purchased_cars->head = store->customerList.customers[i]->purchased_cars->tail = node;
                    }
                    else {
                        store->customerList.customers[i]->purchased_cars->tail->next = node;
                        node->prev = store->customerList.customers[i]->purchased_cars->tail;
                        store->customerList.customers[i]->purchased_cars->tail = node;
                    }
                    store->customerList.customers[i]->purchased_cars->size++;
                }

                // Load purchased parts
                size_t part_count;
                if (fread(&part_count, sizeof(size_t), 1, file) != 1) {
                    PRINT_ERROR("Error reading part purchase count from file.");
                    fclose(file);
                    return 0;
                }
                for (size_t j = 0; j < part_count; j++) {
                    Part* part = loadCompressedPartFromFile(file);
                    if (!part) {
                        PRINT_ERROR("Error reading purchased part data from file.");
                        fclose(file);
                        return 0;
                    }
                    LinkedListNode* node = (LinkedListNode*)malloc(sizeof(LinkedListNode));
                    if (!node) {
                        PRINT_ERROR("Memory allocation failed for linked list node.");
                        free(part);
                        fclose(file);
                        return 0;
                    }
                    node->data = part;
                    node->next = node->prev = NULL;
                    if (!store->customerList.customers[i]->purchased_parts->head) {
                        store->customerList.customers[i]->purchased_parts->head = store->customerList.customers[i]->purchased_parts->tail = node;
                    }
                    else {
                        store->customerList.customers[i]->purchased_parts->tail->next = node;
                        node->prev = store->customerList.customers[i]->purchased_parts->tail;
                        store->customerList.customers[i]->purchased_parts->tail = node;
                    }
                    store->customerList.customers[i]->purchased_parts->size++;
                }
            }
        }

        // Load garage list in binary format
        size_t garageCount = 0;
        if (fread(&garageCount, sizeof(size_t), 1, file) != 1) {
            PRINT_ERROR("Error reading garage count from file.");
            fclose(file);
            return 0;
        }

        for (size_t i = 0; i < garageCount; i++) {
            GarageNode* newNode = (GarageNode*)malloc(sizeof(GarageNode));
            if (!newNode) {
                PRINT_ERROR("Memory allocation failed for garage node.");
                fclose(file);
                return 0;
            }
            if (fread(&newNode->garage, sizeof(Garage), 1, file) != 1) {
                PRINT_ERROR("Error reading garage data from file.");
                free(newNode);
                fclose(file);
                return 0;
            }
            newNode->next = store->garageList->head;
            store->garageList->head = newNode;
        }
    }
    else {
        // Load car inventory in text format
        size_t count;
        char line[MAX_LINE_LEN];
        if (fgets(line, sizeof(line), file)) {
            sscanf(line, "%zu", &count);
            store->carInventory.count = count;
            store->carInventory.cars = (Car**)malloc(count * sizeof(Car*));
            for (size_t i = 0; i < count; i++) {
                if (fgets(line, sizeof(line), file)) {
                    Car* car = (Car*)malloc(sizeof(Car));
                    if (!car) {
                        PRINT_ERROR("Memory allocation failed for car.");
                        continue;
                    }
                    sscanf(line, "%[^,],%[^,],%[^,],%[^,],%f,%d",
                        car->type, car->engine_type, car->brand, car->model, &car->price, &car->serial_number);
                    store->carInventory.cars[i] = car;
                }
            }
        }

        // Load part inventory in text format
        if (fgets(line, sizeof(line), file)) {
            sscanf(line, "%zu", &count);
            store->partInventory.count = count;
            store->partInventory.parts = (Part**)malloc(count * sizeof(Part*));
            for (size_t i = 0; i < count; i++) {
                if (fgets(line, sizeof(line), file)) {
                    Part* part = (Part*)malloc(sizeof(Part));
                    if (!part) {
                        PRINT_ERROR("Memory allocation failed for part.");
                        continue;
                    }
                    char manufacturer[MAX_NAME_LEN];
                    sscanf(line, "%d,%[^,],%[^,],%f,%[^,],%f,%f,%f",
                        &part->serial_number, part->name, part->brand, &part->price, manufacturer,
                        &part->dimensions.length, &part->dimensions.width, &part->dimensions.height);
                    part->manufacturer = _strdup(manufacturer);
                    store->partInventory.parts[i] = part;
                }
            }
        }

        // Load customer list in text format
        if (fgets(line, sizeof(line), file)) {
            sscanf(line, "%zu", &count);
            store->customerList.count = count;
            store->customerList.customers = (Customer**)malloc(count * sizeof(Customer*));
            for (size_t i = 0; i < count; i++) {
                if (fgets(line, sizeof(line), file)) {
                    Customer* customer = (Customer*)malloc(sizeof(Customer));
                    if (!customer) {
                        PRINT_ERROR("Memory allocation failed for customer.");
                        continue;
                    }
                    sscanf(line, "%[^,],%[^,],%s", customer->first_name, customer->last_name, customer->id);
                    customer->purchased_cars = (LinkedList*)malloc(sizeof(LinkedList));
                    customer->purchased_parts = (LinkedList*)malloc(sizeof(LinkedList));
                    customer->purchased_cars->head = customer->purchased_cars->tail = NULL;
                    customer->purchased_parts->head = customer->purchased_parts->tail = NULL;
                    customer->purchased_cars->size = customer->purchased_parts->size = 0;
                    store->customerList.customers[i] = customer;

                    // Load purchased cars
                    if (fgets(line, sizeof(line), file)) {
                        size_t car_count;
                        sscanf(line, "%zu", &car_count);
                        for (size_t j = 0; j < car_count; j++) {
                            if (fgets(line, sizeof(line), file)) {
                                Car* car = (Car*)malloc(sizeof(Car));
                                if (!car) {
                                    PRINT_ERROR("Memory allocation failed for purchased car.");
                                    continue;
                                }
                                sscanf(line, "%[^,],%[^,],%[^,],%[^,],%f,%d",
                                    car->type, car->engine_type, car->brand, car->model, &car->price, &car->serial_number);
                                LinkedListNode* node = (LinkedListNode*)malloc(sizeof(LinkedListNode));
                                if (!node) {
                                    PRINT_ERROR("Memory allocation failed for linked list node.");
                                    free(car);
                                    continue;
                                }
                                node->data = car;
                                node->next = node->prev = NULL;
                                if (!customer->purchased_cars->head) {
                                    customer->purchased_cars->head = customer->purchased_cars->tail = node;
                                }
                                else {
                                    customer->purchased_cars->tail->next = node;
                                    node->prev = customer->purchased_cars->tail;
                                    customer->purchased_cars->tail = node;
                                }
                                customer->purchased_cars->size++;
                            }
                        }
                    }

                    // Load purchased parts
                    if (fgets(line, sizeof(line), file)) {
                        size_t part_count;
                        sscanf(line, "%zu", &part_count);
                        for (size_t j = 0; j < part_count; j++) {
                            if (fgets(line, sizeof(line), file)) {
                                Part* part = (Part*)malloc(sizeof(Part));
                                if (!part) {
                                    PRINT_ERROR("Memory allocation failed for purchased part.");
                                    continue;
                                }
                                char manufacturer[MAX_NAME_LEN];
                                sscanf(line, "%d,%[^,],%[^,],%f,%[^,],%f,%f,%f",
                                    &part->serial_number, part->name, part->brand, &part->price, manufacturer,
                                    &part->dimensions.length, &part->dimensions.width, &part->dimensions.height);
                                part->manufacturer = _strdup(manufacturer);
                                LinkedListNode* node = (LinkedListNode*)malloc(sizeof(LinkedListNode));
                                if (!node) {
                                    PRINT_ERROR("Memory allocation failed for linked list node.");
                                    free(part);
                                    continue;
                                }
                                node->data = part;
                                node->next = node->prev = NULL;
                                if (!customer->purchased_parts->head) {
                                    customer->purchased_parts->head = customer->purchased_parts->tail = node;
                                }
                                else {
                                    customer->purchased_parts->tail->next = node;
                                    node->prev = customer->purchased_parts->tail;
                                    customer->purchased_parts->tail = node;
                                }
                                customer->purchased_parts->size++;
                            }
                        }
                    }
                }
            }
        }

        // Load garage list in text format
        if (fgets(line, sizeof(line), file)) {
            sscanf(line, "%zu", &count);
            for (size_t i = 0; i < count; i++) {
                if (fgets(line, sizeof(line), file)) {
                    GarageNode* newNode = (GarageNode*)malloc(sizeof(GarageNode));
                    if (!newNode) {
                        PRINT_ERROR("Memory allocation failed for garage node.");
                        continue;
                    }
                    sscanf(line, "%d,%[^,],%[^\n]", &newNode->garage.id, newNode->garage.name, newNode->garage.address);
                    newNode->next = store->garageList->head;
                    store->garageList->head = newNode;
                }
            }
        }
    }

    fclose(file);
    printf("Data successfully loaded from %s.\n", filename);
    return 1;
}
