#define _CRT_SECURE_NO_WARNINGS
#include "dashboard.h"
#include "file_operations.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

/*
 * A full-screen terminal dashboard for the car market.
 * It only calls the existing store functions (createCar, purchaseCar, sortCars, findCar,
 * saveToFile, ...), so every number on screen comes from the same data structures as the
 * classic menu. Box-drawing characters are written as UTF-8 escapes so the file compiles
 * the same way with GCC, Clang and MSVC.
 */

#define SCREEN_WIDTH 100

// ---------------------------------------------------------------- colors and symbols

#define RESET "\x1b[0m"
#define BOLD "\x1b[1m"
#define C_ACCENT "\x1b[38;5;214m"
#define C_CYAN "\x1b[38;5;80m"
#define C_GREEN "\x1b[38;5;78m"
#define C_RED "\x1b[38;5;203m"
#define C_TEXT "\x1b[38;5;253m"
#define C_MUTED "\x1b[38;5;245m"
#define C_DIM "\x1b[38;5;239m"
#define C_TITLEBAR "\x1b[48;5;214m\x1b[38;5;16m"

#define S_H "\xE2\x94\x80"
#define S_V "\xE2\x94\x82"
#define S_TL "\xE2\x95\xAD"
#define S_TR "\xE2\x95\xAE"
#define S_BL "\xE2\x95\xB0"
#define S_BR "\xE2\x95\xAF"
#define S_TEE_L "\xE2\x94\x9C"
#define S_TEE_R "\xE2\x94\xA4"
#define S_BLOCK "\xE2\x96\x88"
#define S_SHADE "\xE2\x96\x91"
#define S_DOT "\xE2\x80\xA2"
#define S_ARROW "\xE2\x96\xB8"

typedef enum {
    SCREEN_HOME,
    SCREEN_CARS,
    SCREEN_PARTS,
    SCREEN_CUSTOMERS,
    SCREEN_SELL,
    SCREEN_GARAGES,
    SCREEN_FILES,
    SCREEN_EXIT
} Screen;

static char statusMessage[160] = "";
static int statusIsError = 0;
static int inputEnded = 0;
static const char* carSortLabel = "insertion order";
static const char* partSortLabel = "insertion order";
static const Car* highlightedCar = NULL;
static const Customer* selectedCustomer = NULL;

// ---------------------------------------------------------------- terminal helpers

static void enableTerminalFeatures(void) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (out != INVALID_HANDLE_VALUE && GetConsoleMode(out, &mode)) {
        SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
}

static void clearScreen(void) {
    printf("\x1b[2J\x1b[H");
}

static void setStatus(int isError, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(statusMessage, sizeof(statusMessage), format, args);
    va_end(args);
    statusIsError = isError;
}

// Visible width of a string: skips color escape codes and counts UTF-8 characters once.
static int visibleLength(const char* text) {
    int length = 0;
    for (const unsigned char* p = (const unsigned char*)text; *p; p++) {
        if (*p == 0x1b) {
            while (*p && *p != 'm') p++;
            if (!*p) break;
            continue;
        }
        if ((*p & 0xC0) != 0x80) length++;
    }
    return length;
}

static void repeat(const char* symbol, int count) {
    for (int i = 0; i < count; i++) fputs(symbol, stdout);
}

static void boxTop(const char* title, int width) {
    printf(C_DIM S_TL S_H RESET);
    int used = 2;
    if (title && *title) {
        printf(" " BOLD C_ACCENT "%s" RESET " ", title);
        used += visibleLength(title) + 2;
    }
    printf(C_DIM);
    repeat(S_H, width - used - 1);
    printf(S_TR RESET "\n");
}

static void boxBottom(int width) {
    printf(C_DIM S_BL);
    repeat(S_H, width - 2);
    printf(S_BR RESET "\n");
}

static void boxDivider(int width) {
    printf(C_DIM S_TEE_L);
    repeat(S_H, width - 2);
    printf(S_TEE_R RESET "\n");
}

// One line inside a box; the content may contain color codes.
static void boxRow(int width, const char* format, ...) {
    char content[512];
    va_list args;
    va_start(args, format);
    vsnprintf(content, sizeof(content), format, args);
    va_end(args);
    int padding = width - 4 - visibleLength(content);
    printf(C_DIM S_V RESET " %s", content);
    if (padding > 0) printf("%*s", padding, "");
    printf(" " C_DIM S_V RESET "\n");
}

// Pads (or truncates) plain text to an exact width, for table columns.
static void cell(char* out, size_t size, const char* text, int width) {
    snprintf(out, size, "%-*.*s", width, width, text ? text : "");
}

static void formatMoney(char* out, size_t size, double value) {
    long long whole = (long long)(value + 0.5);
    char digits[32];
    snprintf(digits, sizeof(digits), "%lld", whole);
    int len = (int)strlen(digits);
    char grouped[48];
    int j = 0;
    for (int i = 0; i < len; i++) {
        grouped[j++] = digits[i];
        int remaining = len - i - 1;
        if (remaining > 0 && remaining % 3 == 0) grouped[j++] = ',';
    }
    grouped[j] = '\0';
    snprintf(out, size, "$%s", grouped);
}

// ---------------------------------------------------------------- input helpers

static int readLine(const char* prompt, char* buffer, size_t size) {
    printf(C_ACCENT S_ARROW RESET " %s" C_ACCENT, prompt);
    fflush(stdout);
    if (!fgets(buffer, (int)size, stdin)) {
        printf(RESET "\n");
        inputEnded = 1;
        buffer[0] = '\0';
        return 0;
    }
    printf(RESET);
    size_t length = strcspn(buffer, "\n");
    if (buffer[length] != '\n') {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }
    buffer[length] = '\0';
    return 1;
}

static int readText(const char* prompt, char* buffer, size_t size) {
    if (!readLine(prompt, buffer, size)) return 0;
    if (strlen(buffer) == 0) {
        setStatus(1, "This field cannot be empty.");
        return 0;
    }
    if (strchr(buffer, ',')) {
        setStatus(1, "Commas are not allowed, because the data file uses them as separators.");
        return 0;
    }
    return 1;
}

static int readPositiveFloat(const char* prompt, float* value) {
    char buffer[INPUT_BUFFER_SIZE];
    char extra;
    if (!readLine(prompt, buffer, sizeof(buffer))) return 0;
    if (sscanf(buffer, "%f %c", value, &extra) != 1 || *value <= 0) {
        setStatus(1, "Please enter a positive number.");
        return 0;
    }
    return 1;
}

static int readInt(const char* prompt, int* value) {
    char buffer[INPUT_BUFFER_SIZE];
    char extra;
    if (!readLine(prompt, buffer, sizeof(buffer))) return 0;
    if (sscanf(buffer, "%d %c", value, &extra) != 1) {
        setStatus(1, "Please enter a whole number.");
        return 0;
    }
    return 1;
}

static int readCustomerId(const char* prompt, char* id) {
    char buffer[INPUT_BUFFER_SIZE];
    if (!readLine(prompt, buffer, sizeof(buffer))) return 0;
    if (strlen(buffer) != MAX_CUSTOMER_ID_LEN) {
        setStatus(1, "The ID must be exactly %d digits.", MAX_CUSTOMER_ID_LEN);
        return 0;
    }
    for (int i = 0; i < MAX_CUSTOMER_ID_LEN; i++) {
        if (!isdigit((unsigned char)buffer[i])) {
            setStatus(1, "The ID must contain digits only.");
            return 0;
        }
    }
    strcpy(id, buffer);
    return 1;
}

static Customer* findCustomer(StoreManager* store, const char* id) {
    for (int i = 0; i < store->customerList.count; i++) {
        if (strcmp(store->customerList.customers[i]->id, id) == 0) {
            return store->customerList.customers[i];
        }
    }
    return NULL;
}

// ---------------------------------------------------------------- statistics

typedef struct {
    int salesCount;
    double revenue;
    double stockValue;
    int garageCount;
} Stats;

static Stats computeStats(const StoreManager* store) {
    Stats stats = { 0, 0.0, 0.0, 0 };
    for (int i = 0; i < store->carInventory.count; i++) {
        stats.stockValue += store->carInventory.cars[i]->price;
    }
    for (int i = 0; i < store->partInventory.count; i++) {
        stats.stockValue += store->partInventory.parts[i]->price;
    }
    for (int i = 0; i < store->customerList.count; i++) {
        const Customer* customer = store->customerList.customers[i];
        for (LinkedListNode* n = customer->purchased_cars->head; n; n = n->next) {
            stats.salesCount++;
            stats.revenue += ((Car*)n->data)->price;
        }
        for (LinkedListNode* n = customer->purchased_parts->head; n; n = n->next) {
            stats.salesCount++;
            stats.revenue += ((Part*)n->data)->price;
        }
    }
    for (GarageNode* g = store->garageList->head; g; g = g->next) {
        stats.garageCount++;
    }
    return stats;
}

static double customerSpending(const Customer* customer, int* items) {
    double total = 0;
    *items = 0;
    for (LinkedListNode* n = customer->purchased_cars->head; n; n = n->next) {
        total += ((Car*)n->data)->price;
        (*items)++;
    }
    for (LinkedListNode* n = customer->purchased_parts->head; n; n = n->next) {
        total += ((Part*)n->data)->price;
        (*items)++;
    }
    return total;
}

// ---------------------------------------------------------------- shared layout

static void drawHeader(const StoreManager* store, const char* section) {
    Stats stats = computeStats(store);
    char value[32];
    char revenue[32];
    formatMoney(value, sizeof(value), stats.stockValue);
    formatMoney(revenue, sizeof(revenue), stats.revenue);

    char left[64];
    snprintf(left, sizeof(left), "  CAR MARKET  " S_DOT "  %s", section);
    printf(C_TITLEBAR BOLD "%s", left);
    printf("%*s" RESET "\n", SCREEN_WIDTH - visibleLength(left), "Car Management System  ");
    printf(C_MUTED "  %d cars " C_DIM S_DOT C_MUTED " %d parts " C_DIM S_DOT C_MUTED " %d customers " C_DIM S_DOT
        C_MUTED " %d garages " C_DIM S_DOT C_MUTED " stock value " C_TEXT "%s" C_DIM " " S_DOT C_MUTED
        " sales revenue " C_GREEN "%s" RESET "\n\n",
        store->carInventory.count, store->partInventory.count, store->customerList.count, stats.garageCount,
        value, revenue);
}

static void drawStatus(void) {
    if (statusMessage[0]) {
        printf("  %s%s %s" RESET "\n", statusIsError ? C_RED : C_GREEN, statusIsError ? "!" : S_DOT, statusMessage);
        statusMessage[0] = '\0';
    }
    else {
        printf("\n");
    }
}

static void drawKeys(const char* keys) {
    printf(C_DIM);
    repeat(S_H, SCREEN_WIDTH);
    printf(RESET "\n  %s\n", keys);
}

// Formats "[k] label" pairs: "k:label|k:label"
static void keyBar(char* out, size_t size, const char* pairs) {
    out[0] = '\0';
    char copy[256];
    strncpy(copy, pairs, sizeof(copy) - 1);
    copy[sizeof(copy) - 1] = '\0';
    for (char* item = strtok(copy, "|"); item; item = strtok(NULL, "|")) {
        char* colon = strchr(item, ':');
        if (!colon) continue;
        *colon = '\0';
        char piece[64];
        snprintf(piece, sizeof(piece), C_ACCENT "[%s]" C_TEXT " %s  " RESET, item, colon + 1);
        strncat(out, piece, size - strlen(out) - 1);
    }
}

// ---------------------------------------------------------------- tables

static void carTableHeader(int width) {
    boxRow(width, C_MUTED "%-11s %-12s %-16s %-10s %-10s %14s" RESET, "SERIAL", "BRAND", "MODEL", "TYPE", "ENGINE",
        "PRICE");
    boxDivider(width);
}

static void carTableRow(int width, const Car* car, int highlight) {
    char brand[16], model[20], type[16], engine[16], price[32];
    cell(brand, sizeof(brand), car->brand, 12);
    cell(model, sizeof(model), car->model, 16);
    cell(type, sizeof(type), car->type, 10);
    cell(engine, sizeof(engine), car->engine_type, 10);
    formatMoney(price, sizeof(price), car->price);
    boxRow(width, "%s%-11d %s %s %s %s %14s" RESET, highlight ? C_ACCENT BOLD : C_TEXT, car->serial_number, brand,
        model, type, engine, price);
}

static void partTableHeader(int width) {
    boxRow(width, C_MUTED "%-11s %-16s %-9s %-19s %-18s %10s" RESET, "SERIAL", "PART", "BRAND", "MANUFACTURER",
        "SIZE (L x W x H)", "PRICE");
    boxDivider(width);
}

static void partTableRow(int width, const Part* part) {
    char name[20], brand[16], maker[24], size[48], price[32];
    cell(name, sizeof(name), part->name, 16);
    cell(brand, sizeof(brand), part->brand, 9);
    cell(maker, sizeof(maker), part->manufacturer, 19);
    snprintf(size, sizeof(size), "%.1f x %.1f x %.1f", part->dimensions.length, part->dimensions.width,
        part->dimensions.height);
    formatMoney(price, sizeof(price), part->price);
    boxRow(width, C_TEXT "%-11d %s %s %s %-18.18s %10s" RESET, part->serial_number, name, brand, maker, size, price);
}

// ---------------------------------------------------------------- home screen

typedef struct {
    char brand[MAX_BRAND_LEN];
    int count;
} BrandCount;

static int compareBrandCount(const void* a, const void* b) {
    const BrandCount* x = (const BrandCount*)a;
    const BrandCount* y = (const BrandCount*)b;
    if (y->count != x->count) return y->count - x->count;
    return strcmp(x->brand, y->brand);
}

static void drawTiles(const StoreManager* store, const Stats* stats) {
    const int tileWidth = 24;
    char money[32];
    char lines[4][4][160];
    const char* titles[4] = { "CARS IN STOCK", "PARTS IN STOCK", "CUSTOMERS", "ITEMS SOLD" };
    char values[4][32];
    char notes[4][48];

    snprintf(values[0], sizeof(values[0]), "%d", store->carInventory.count);
    snprintf(values[1], sizeof(values[1]), "%d", store->partInventory.count);
    snprintf(values[2], sizeof(values[2]), "%d", store->customerList.count);
    snprintf(values[3], sizeof(values[3]), "%d", stats->salesCount);
    formatMoney(money, sizeof(money), stats->stockValue);
    snprintf(notes[0], sizeof(notes[0]), "dynamic array");
    snprintf(notes[1], sizeof(notes[1]), "dynamic array");
    snprintf(notes[2], sizeof(notes[2]), "with purchase lists");
    formatMoney(money, sizeof(money), stats->revenue);
    snprintf(notes[3], sizeof(notes[3]), "%s revenue", money);

    for (int t = 0; t < 4; t++) {
        snprintf(lines[t][0], sizeof(lines[t][0]), C_DIM S_TL S_H RESET " " C_MUTED "%s" RESET " ", titles[t]);
        snprintf(lines[t][1], sizeof(lines[t][1]), C_DIM S_V RESET "  " BOLD C_ACCENT "%s" RESET, values[t]);
        snprintf(lines[t][2], sizeof(lines[t][2]), C_DIM S_V RESET "  " C_MUTED "%s" RESET, notes[t]);
        snprintf(lines[t][3], sizeof(lines[t][3]), C_DIM S_BL RESET);
    }
    for (int row = 0; row < 4; row++) {
        for (int t = 0; t < 4; t++) {
            const char* text = lines[t][row];
            int pad = tileWidth - 1 - visibleLength(text);
            printf("%s", text);
            if (row == 0) {
                printf(C_DIM);
                repeat(S_H, pad);
                printf(S_TR RESET);
            }
            else if (row == 3) {
                printf(C_DIM);
                repeat(S_H, pad);
                printf(S_BR RESET);
            }
            else {
                printf("%*s" C_DIM S_V RESET, pad, "");
            }
            if (t < 3) printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

static void drawHome(StoreManager* store) {
    Stats stats = computeStats(store);
    drawHeader(store, "Dashboard");
    drawTiles(store, &stats);

    // brands in stock, as a bar chart
    BrandCount brands[64];
    int brandCount = 0;
    for (int i = 0; i < store->carInventory.count && brandCount < 64; i++) {
        const char* brand = store->carInventory.cars[i]->brand;
        int found = 0;
        for (int b = 0; b < brandCount; b++) {
            if (strcmp(brands[b].brand, brand) == 0) {
                brands[b].count++;
                found = 1;
                break;
            }
        }
        if (!found) {
            strncpy(brands[brandCount].brand, brand, MAX_BRAND_LEN - 1);
            brands[brandCount].brand[MAX_BRAND_LEN - 1] = '\0';
            brands[brandCount].count = 1;
            brandCount++;
        }
    }
    qsort(brands, brandCount, sizeof(BrandCount), compareBrandCount);
    int maxCount = brandCount > 0 ? brands[0].count : 1;

    const int width = 48;
    char leftLines[10][512];
    int leftCount = 0;
    for (int b = 0; b < brandCount && b < 7; b++) {
        int bar = brands[b].count * 22 / maxCount;
        if (bar < 1) bar = 1;
        char barText[160] = "";
        for (int k = 0; k < bar; k++) strcat(barText, S_BLOCK);
        char shade[160] = "";
        for (int k = bar; k < 22; k++) strcat(shade, S_SHADE);
        char brand[16];
        cell(brand, sizeof(brand), brands[b].brand, 12);
        snprintf(leftLines[leftCount++], sizeof(leftLines[0]), C_TEXT "%s " C_ACCENT "%s" C_DIM "%s " C_TEXT "%d" RESET,
            brand, barText, shade, brands[b].count);
    }
    if (leftCount == 0) {
        snprintf(leftLines[leftCount++], sizeof(leftLines[0]), C_MUTED "No cars in stock. Load a file or add cars." RESET);
    }

    // latest purchases
    char rightLines[10][256];
    int rightCount = 0;
    for (int i = store->customerList.count - 1; i >= 0 && rightCount < 7; i--) {
        const Customer* c = store->customerList.customers[i];
        for (LinkedListNode* n = c->purchased_cars->tail; n && rightCount < 7; n = n->prev) {
            const Car* car = (const Car*)n->data;
            char who[16], what[24], price[24], item[MAX_BRAND_LEN + MAX_MODEL_LEN];
            snprintf(item, sizeof(item), "%s %s", car->brand, car->model);
            cell(who, sizeof(who), c->first_name, 9);
            cell(what, sizeof(what), item, 18);
            formatMoney(price, sizeof(price), car->price);
            snprintf(rightLines[rightCount++], sizeof(rightLines[0]), C_TEXT "%s " C_CYAN "car  " C_TEXT "%s " C_GREEN "%9s" RESET,
                who, what, price);
        }
        for (LinkedListNode* n = c->purchased_parts->tail; n && rightCount < 7; n = n->prev) {
            const Part* part = (const Part*)n->data;
            char who[16], what[24], price[24];
            cell(who, sizeof(who), c->first_name, 9);
            cell(what, sizeof(what), part->name, 18);
            formatMoney(price, sizeof(price), part->price);
            snprintf(rightLines[rightCount++], sizeof(rightLines[0]), C_TEXT "%s " C_CYAN "part " C_TEXT "%s " C_GREEN "%9s" RESET,
                who, what, price);
        }
    }
    if (rightCount == 0) {
        snprintf(rightLines[rightCount++], sizeof(rightLines[0]), C_MUTED "No sales yet." RESET);
    }

    // two boxes side by side
    int rows = leftCount > rightCount ? leftCount : rightCount;
    if (rows < 5) rows = 5;
    char top[2][512];
    snprintf(top[0], sizeof(top[0]), "Cars in stock by brand");
    snprintf(top[1], sizeof(top[1]), "Latest sales");
    for (int side = 0; side < 2; side++) {
        printf(C_DIM S_TL S_H RESET " " BOLD C_ACCENT "%s" RESET " " C_DIM, top[side]);
        repeat(S_H, width - visibleLength(top[side]) - 5);
        printf(S_TR RESET);
        printf(side == 0 ? "   " : "\n");
    }
    for (int r = 0; r < rows; r++) {
        for (int side = 0; side < 2; side++) {
            const char* text = side == 0 ? (r < leftCount ? leftLines[r] : "") : (r < rightCount ? rightLines[r] : "");
            int pad = width - 4 - visibleLength(text);
            printf(C_DIM S_V RESET " %s%*s " C_DIM S_V RESET, text, pad > 0 ? pad : 0, "");
            printf(side == 0 ? "   " : "\n");
        }
    }
    for (int side = 0; side < 2; side++) {
        printf(C_DIM S_BL);
        repeat(S_H, width - 2);
        printf(S_BR RESET);
        printf(side == 0 ? "   " : "\n");
    }
    printf("\n");
}

static Screen homeScreen(StoreManager* store) {
    char keys[512], input[INPUT_BUFFER_SIZE];
    clearScreen();
    drawHome(store);
    drawStatus();
    keyBar(keys, sizeof(keys), "1:Cars|2:Parts|3:Customers|4:Sell|5:Garages|6:Files|0:Exit");
    drawKeys(keys);
    if (!readLine("Choose a section: ", input, sizeof(input))) return SCREEN_EXIT;
    switch (input[0]) {
    case '1': return SCREEN_CARS;
    case '2': return SCREEN_PARTS;
    case '3': return SCREEN_CUSTOMERS;
    case '4': return SCREEN_SELL;
    case '5': return SCREEN_GARAGES;
    case '6': return SCREEN_FILES;
    case '0': return SCREEN_EXIT;
    default:
        setStatus(1, "Unknown option.");
        return SCREEN_HOME;
    }
}

// ---------------------------------------------------------------- cars screen

static void addCarFlow(StoreManager* store) {
    char type[MAX_TYPE_LEN], engine[MAX_ENGINE_TYPE_LEN], brand[MAX_BRAND_LEN], model[MAX_MODEL_LEN];
    float price;
    if (!readText("Type (Sedan, SUV, ...): ", type, sizeof(type))) return;
    if (!readText("Engine (Petrol, Diesel, Electric, ...): ", engine, sizeof(engine))) return;
    if (!readText("Brand: ", brand, sizeof(brand))) return;
    if (!readText("Model: ", model, sizeof(model))) return;
    if (!readPositiveFloat("Price: ", &price)) return;
    Car* car = createCar(type, engine, brand, model, price, &store->carInventory);
    if (car && addCar(store, car)) {
        highlightedCar = car;
        setStatus(0, "Added %s %s with serial number %d.", brand, model, car->serial_number);
    }
    else {
        free(car);
        setStatus(1, "Could not add the car.");
    }
}

static void searchCarFlow(StoreManager* store) {
    char input[INPUT_BUFFER_SIZE];
    if (!readLine("Search by [s] serial, [b] brand or [p] price: ", input, sizeof(input))) return;
    Car* car = NULL;
    // bsearch needs a sorted array, so sort by the same key first (like the classic menu)
    if (input[0] == 's') {
        int serial;
        if (!readInt("Serial number: ", &serial)) return;
        sortCars(&store->carInventory, compareBySerialNumber);
        carSortLabel = "serial number";
        car = findCar(&store->carInventory, &serial, searchBySerialNumber);
    }
    else if (input[0] == 'b') {
        char brand[MAX_BRAND_LEN];
        if (!readText("Brand: ", brand, sizeof(brand))) return;
        sortCars(&store->carInventory, compareByBrand);
        carSortLabel = "brand";
        car = findCar(&store->carInventory, brand, searchByBrand);
    }
    else if (input[0] == 'p') {
        float price;
        if (!readPositiveFloat("Exact price: ", &price)) return;
        sortCars(&store->carInventory, compareByPrice);
        carSortLabel = "price";
        car = findCar(&store->carInventory, &price, searchByPrice);
    }
    else {
        setStatus(1, "Unknown search option.");
        return;
    }
    highlightedCar = car;
    if (car) {
        setStatus(0, "Found %s %s (serial %d) with bsearch, highlighted in the table.", car->brand,
            car->model, car->serial_number);
    }
    else {
        setStatus(1, "No car matches that search.");
    }
}

static void increasePricesFlow(StoreManager* store) {
    char input[INPUT_BUFFER_SIZE];
    if (store->carInventory.count == 0) {
        setStatus(1, "There are no cars to update.");
        return;
    }
    double before = 0, after = 0;
    for (int i = 0; i < store->carInventory.count; i++) before += store->carInventory.cars[i]->price;
    if (!readLine("Raise every car price by 15%? [y/n]: ", input, sizeof(input))) return;
    if (input[0] != 'y' && input[0] != 'Y') {
        setStatus(0, "Prices were not changed.");
        return;
    }
    increaseAllCarPrices(&store->carInventory);
    for (int i = 0; i < store->carInventory.count; i++) after += store->carInventory.cars[i]->price;
    char a[32], b[32];
    formatMoney(a, sizeof(a), before);
    formatMoney(b, sizeof(b), after);
    setStatus(0, "All car prices raised by 15%%. Total car stock value: %s -> %s.", a, b);
}

static Screen carsScreen(StoreManager* store) {
    char keys[512], input[INPUT_BUFFER_SIZE];
    const int width = SCREEN_WIDTH;
    clearScreen();
    drawHeader(store, "Cars");
    char title[96];
    snprintf(title, sizeof(title), "Car inventory  " C_MUTED "(sorted by %s)", carSortLabel);
    boxTop(title, width);
    carTableHeader(width);
    if (store->carInventory.count == 0) {
        boxRow(width, C_MUTED "No cars in the inventory." RESET);
    }
    for (int i = 0; i < store->carInventory.count; i++) {
        carTableRow(width, store->carInventory.cars[i], store->carInventory.cars[i] == highlightedCar);
    }
    boxBottom(width);
    drawStatus();
    keyBar(keys, sizeof(keys), "a:Add|r:Remove|p:Price|b:Brand|s:Serial|f:Find|+:Raise 15%|0:Back");
    drawKeys(keys);
    if (!readLine("Action: ", input, sizeof(input))) return SCREEN_EXIT;

    switch (input[0]) {
    case 'a': addCarFlow(store); break;
    case 'r': {
        int serial;
        if (readInt("Serial number to remove: ", &serial)) {
            if (removeCar(store, serial)) {
                highlightedCar = NULL;
                setStatus(0, "Car %d was removed.", serial);
            }
            else {
                setStatus(1, "No car with serial number %d.", serial);
            }
        }
        break;
    }
    case 'p': sortCars(&store->carInventory, compareByPrice); carSortLabel = "price"; setStatus(0, "Sorted with qsort by price."); break;
    case 'b': sortCars(&store->carInventory, compareByBrand); carSortLabel = "brand"; setStatus(0, "Sorted with qsort by brand."); break;
    case 's': sortCars(&store->carInventory, compareBySerialNumber); carSortLabel = "serial number"; setStatus(0, "Sorted with qsort by serial number."); break;
    case 'f': searchCarFlow(store); break;
    case '+': increasePricesFlow(store); break;
    case '0': highlightedCar = NULL; return SCREEN_HOME;
    default: setStatus(1, "Unknown action.");
    }
    return SCREEN_CARS;
}

// ---------------------------------------------------------------- parts screen

static void addPartFlow(StoreManager* store) {
    char name[MAX_NAME_LEN], brand[MAX_BRAND_LEN], maker[MAX_NAME_LEN];
    char input[INPUT_BUFFER_SIZE];
    float price, length, width, height;
    char extra;
    if (!readText("Part name: ", name, sizeof(name))) return;
    if (!readText("Brand: ", brand, sizeof(brand))) return;
    if (!readPositiveFloat("Price: ", &price)) return;
    if (!readText("Manufacturer: ", maker, sizeof(maker))) return;
    if (!readLine("Size as length width height: ", input, sizeof(input))) return;
    if (sscanf(input, "%f %f %f %c", &length, &width, &height, &extra) != 3 || length <= 0 || width <= 0 || height <= 0) {
        setStatus(1, "Enter exactly three positive numbers separated by spaces.");
        return;
    }
    Part* part = createPart(name, brand, price, maker, length, width, height, &store->partInventory);
    if (part && addPart(store, part)) {
        setStatus(0, "Added %s with serial number %d.", name, part->serial_number);
    }
    else {
        freePart(part);
        setStatus(1, "Could not add the part.");
    }
}

static Screen partsScreen(StoreManager* store) {
    char keys[512], input[INPUT_BUFFER_SIZE];
    const int width = SCREEN_WIDTH;
    clearScreen();
    drawHeader(store, "Parts");
    char title[96];
    snprintf(title, sizeof(title), "Parts inventory  " C_MUTED "(sorted by %s)", partSortLabel);
    boxTop(title, width);
    partTableHeader(width);
    if (store->partInventory.count == 0) {
        boxRow(width, C_MUTED "No parts in the inventory." RESET);
    }
    for (int i = 0; i < store->partInventory.count; i++) {
        partTableRow(width, store->partInventory.parts[i]);
    }
    boxBottom(width);
    drawStatus();
    keyBar(keys, sizeof(keys), "a:Add|r:Remove|p:Sort by price|b:Sort by brand|f:Find by serial|0:Back");
    drawKeys(keys);
    if (!readLine("Action: ", input, sizeof(input))) return SCREEN_EXIT;

    switch (input[0]) {
    case 'a': addPartFlow(store); break;
    case 'r': {
        int serial;
        if (readInt("Serial number to remove: ", &serial)) {
            if (removePart(store, serial)) setStatus(0, "Part %d was removed.", serial);
            else setStatus(1, "No part with serial number %d.", serial);
        }
        break;
    }
    case 'p': sortParts(&store->partInventory, comparePartsByPrice); partSortLabel = "price"; setStatus(0, "Sorted with qsort by price."); break;
    case 'b': sortParts(&store->partInventory, comparePartsByBrand); partSortLabel = "brand"; setStatus(0, "Sorted with qsort by brand."); break;
    case 'f': {
        int serial;
        if (readInt("Serial number: ", &serial)) {
            Part* part = searchPart(&store->partInventory, serial);
            if (part) setStatus(0, "Found %s by %s, %.2f.", part->name, part->brand, part->price);
            else setStatus(1, "No part with serial number %d.", serial);
        }
        break;
    }
    case '0': return SCREEN_HOME;
    default: setStatus(1, "Unknown action.");
    }
    return SCREEN_PARTS;
}

// ---------------------------------------------------------------- customers screen

static void drawCustomerDetail(const Customer* customer) {
    const int width = SCREEN_WIDTH;
    char title[128];
    snprintf(title, sizeof(title), "Purchases of %s %s  " C_MUTED "(ID %s)", customer->first_name, customer->last_name,
        customer->id);
    boxTop(title, width);
    int items = 0;
    double total = customerSpending(customer, &items);
    if (items == 0) {
        boxRow(width, C_MUTED "No purchases yet." RESET);
    }
    for (LinkedListNode* n = customer->purchased_cars->head; n; n = n->next) {
        const Car* car = (const Car*)n->data;
        char price[32];
        formatMoney(price, sizeof(price), car->price);
        boxRow(width, C_CYAN "car   " C_TEXT "%-10d %-12.12s %-16.16s %-10.10s %-10.10s" C_GREEN "%16s" RESET,
            car->serial_number, car->brand, car->model, car->type, car->engine_type, price);
    }
    for (LinkedListNode* n = customer->purchased_parts->head; n; n = n->next) {
        const Part* part = (const Part*)n->data;
        char price[32];
        formatMoney(price, sizeof(price), part->price);
        boxRow(width, C_CYAN "part  " C_TEXT "%-10d %-12.12s %-16.16s %-21.21s" C_GREEN "%16s" RESET,
            part->serial_number, part->brand, part->name, part->manufacturer, price);
    }
    char sum[32];
    formatMoney(sum, sizeof(sum), total);
    boxDivider(width);
    boxRow(width, C_MUTED "%d items" C_TEXT "%*s" BOLD C_GREEN "%s" RESET, items, width - 20 - (int)strlen(sum),
        "Total ", sum);
    boxBottom(width);
}

static Screen customersScreen(StoreManager* store) {
    char keys[512], input[INPUT_BUFFER_SIZE];
    const int width = SCREEN_WIDTH;
    clearScreen();
    drawHeader(store, "Customers");
    boxTop("Customers", width);
    boxRow(width, C_MUTED "%-11s %-16s %-16s %10s %16s" RESET, "ID", "FIRST NAME", "LAST NAME", "ITEMS", "TOTAL SPENT");
    boxDivider(width);
    if (store->customerList.count == 0) {
        boxRow(width, C_MUTED "No customers yet." RESET);
    }
    for (int i = 0; i < store->customerList.count; i++) {
        const Customer* c = store->customerList.customers[i];
        int items;
        double total = customerSpending(c, &items);
        char spent[32], first[20], last[20];
        formatMoney(spent, sizeof(spent), total);
        cell(first, sizeof(first), c->first_name, 16);
        cell(last, sizeof(last), c->last_name, 16);
        boxRow(width, "%s%-11s %s %s %10d %16s" RESET, c == selectedCustomer ? C_ACCENT BOLD : C_TEXT, c->id, first, last,
            items, spent);
    }
    boxBottom(width);
    if (selectedCustomer) {
        drawCustomerDetail(selectedCustomer);
    }
    drawStatus();
    keyBar(keys, sizeof(keys), "a:Add customer|v:View purchases|d:Delete customer|0:Back");
    drawKeys(keys);
    if (!readLine("Action: ", input, sizeof(input))) return SCREEN_EXIT;

    switch (input[0]) {
    case 'a': {
        char first[MAX_CUSTOMER_NAME_LEN], last[MAX_CUSTOMER_NAME_LEN], id[MAX_CUSTOMER_ID_LEN + 1];
        if (!readText("First name: ", first, sizeof(first))) break;
        if (!readText("Last name: ", last, sizeof(last))) break;
        if (!readCustomerId("ID (9 digits): ", id)) break;
        if (findCustomer(store, id)) {
            setStatus(1, "A customer with ID %s already exists.", id);
            break;
        }
        Customer* customer = createCustomer(first, last, id);
        if (customer && addCustomer(store, customer)) setStatus(0, "Customer %s %s was added.", first, last);
        else setStatus(1, "Could not add the customer.");
        break;
    }
    case 'v': {
        char id[MAX_CUSTOMER_ID_LEN + 1];
        if (readCustomerId("Customer ID: ", id)) {
            selectedCustomer = findCustomer(store, id);
            if (!selectedCustomer) setStatus(1, "No customer with ID %s.", id);
        }
        break;
    }
    case 'd': {
        char id[MAX_CUSTOMER_ID_LEN + 1];
        if (readCustomerId("Customer ID to delete: ", id)) {
            Customer* customer = findCustomer(store, id);
            if (!customer) {
                setStatus(1, "No customer with ID %s.", id);
                break;
            }
            if (customer == selectedCustomer) selectedCustomer = NULL;
            removeCustomer(store, id);
            setStatus(0, "Customer %s was deleted together with their purchase history.", id);
        }
        break;
    }
    case '0': selectedCustomer = NULL; return SCREEN_HOME;
    default: setStatus(1, "Unknown action.");
    }
    return SCREEN_CUSTOMERS;
}

// ---------------------------------------------------------------- sell screen

static Screen sellScreen(StoreManager* store) {
    char keys[512], input[INPUT_BUFFER_SIZE];
    const int width = SCREEN_WIDTH;
    clearScreen();
    drawHeader(store, "Sell");
    boxTop("New sale", width);
    boxRow(width, C_TEXT "1. Choose the customer by ID." RESET);
    boxRow(width, C_TEXT "2. Choose a car or a part by serial number." RESET);
    boxRow(width, C_MUTED "The item moves from the inventory to the customer's purchase list (a doubly linked list)." RESET);
    boxBottom(width);
    if (selectedCustomer) {
        drawCustomerDetail(selectedCustomer);
    }
    drawStatus();
    keyBar(keys, sizeof(keys), "c:Sell a car|p:Sell a part|0:Back");
    drawKeys(keys);
    if (!readLine("Action: ", input, sizeof(input))) return SCREEN_EXIT;
    if (input[0] == '0') {
        selectedCustomer = NULL;
        return SCREEN_HOME;
    }
    if (input[0] != 'c' && input[0] != 'p') {
        setStatus(1, "Unknown action.");
        return SCREEN_SELL;
    }

    char id[MAX_CUSTOMER_ID_LEN + 1];
    if (!readCustomerId("Customer ID: ", id)) return SCREEN_SELL;
    Customer* customer = findCustomer(store, id);
    if (!customer) {
        setStatus(1, "No customer with ID %s. Add the customer first.", id);
        return SCREEN_SELL;
    }
    int serial;
    if (input[0] == 'c') {
        if (!readInt("Car serial number: ", &serial)) return SCREEN_SELL;
        Car* car = NULL;
        for (int i = 0; i < store->carInventory.count; i++) {
            if (store->carInventory.cars[i]->serial_number == serial) car = store->carInventory.cars[i];
        }
        char label[96] = "";
        if (car) snprintf(label, sizeof(label), "%s %s", car->brand, car->model);
        highlightedCar = NULL;
        if (purchaseCar(store, customer, serial)) setStatus(0, "Sold the %s to %s %s.", label, customer->first_name, customer->last_name);
        else setStatus(1, "No car with serial number %d in stock.", serial);
    }
    else {
        if (!readInt("Part serial number: ", &serial)) return SCREEN_SELL;
        Part* part = searchPart(&store->partInventory, serial);
        char label[64] = "";
        if (part) snprintf(label, sizeof(label), "%s", part->name);
        if (purchasePart(store, customer, serial)) setStatus(0, "Sold %s to %s %s.", label, customer->first_name, customer->last_name);
        else setStatus(1, "No part with serial number %d in stock.", serial);
    }
    selectedCustomer = customer;
    return SCREEN_SELL;
}

// ---------------------------------------------------------------- garages screen

static Screen garagesScreen(StoreManager* store) {
    char keys[512], input[INPUT_BUFFER_SIZE];
    const int width = SCREEN_WIDTH;
    clearScreen();
    drawHeader(store, "Garages");
    boxTop("Partner garages  " C_MUTED "(singly linked list)", width);
    boxRow(width, C_MUTED "%-8s %-30s %-50s" RESET, "ID", "NAME", "ADDRESS");
    boxDivider(width);
    if (!store->garageList->head) {
        boxRow(width, C_MUTED "No garages yet." RESET);
    }
    for (GarageNode* g = store->garageList->head; g; g = g->next) {
        boxRow(width, C_TEXT "%-8d %-30.30s %-50.50s" RESET, g->garage.id, g->garage.name, g->garage.address);
    }
    boxBottom(width);
    drawStatus();
    keyBar(keys, sizeof(keys), "a:Add garage|r:Remove garage|0:Back");
    drawKeys(keys);
    if (!readLine("Action: ", input, sizeof(input))) return SCREEN_EXIT;

    switch (input[0]) {
    case 'a': {
        Garage garage;
        if (!readInt("Garage ID: ", &garage.id)) break;
        if (!readText("Name: ", garage.name, sizeof(garage.name))) break;
        char address[MAX_GARAGE_ADDRESS_LEN];
        // addresses may contain commas: the garage line is read up to the end of the line
        if (!readLine("Address: ", address, sizeof(address)) || strlen(address) == 0) {
            setStatus(1, "The address cannot be empty.");
            break;
        }
        strcpy(garage.address, address);
        if (addGarage(store->garageList, garage)) setStatus(0, "Garage %s was added.", garage.name);
        else setStatus(1, "A garage with ID %d already exists.", garage.id);
        break;
    }
    case 'r': {
        int id;
        if (readInt("Garage ID to remove: ", &id)) {
            if (removeGarage(store->garageList, id)) setStatus(0, "Garage %d was removed.", id);
            else setStatus(1, "No garage with ID %d.", id);
        }
        break;
    }
    case '0': return SCREEN_HOME;
    default: setStatus(1, "Unknown action.");
    }
    return SCREEN_GARAGES;
}

// ---------------------------------------------------------------- files screen

static Screen filesScreen(StoreManager* store) {
    char keys[512], input[INPUT_BUFFER_SIZE];
    const int width = SCREEN_WIDTH;
    clearScreen();
    drawHeader(store, "Files");
    boxTop("Save and load", width);
    boxRow(width, C_TEXT "Text file    " C_MUTED "system_data.txt   readable CSV lines, one record per line" RESET);
    boxRow(width, C_TEXT "Binary file  " C_MUTED "system_data.bin   raw structs; part manufacturers stored compactly" RESET);
    boxDivider(width);
    boxRow(width, C_MUTED "Loading replaces everything currently in memory with the contents of the file." RESET);
    boxBottom(width);
    drawStatus();
    keyBar(keys, sizeof(keys), "1:Load text|2:Load binary|3:Save text|4:Save binary|0:Back");
    drawKeys(keys);
    if (!readLine("Action: ", input, sizeof(input))) return SCREEN_EXIT;

    int ok;
    switch (input[0]) {
    case '1':
    case '2':
        highlightedCar = NULL;
        selectedCustomer = NULL;
        ok = loadFromFile(input[0] == '1' ? "system_data.txt" : "system_data.bin", store, input[0] == '2');
        setStatus(!ok, ok ? "Data loaded." : "Could not load the file.");
        break;
    case '3':
    case '4':
        ok = saveToFile(input[0] == '3' ? "system_data.txt" : "system_data.bin", store, input[0] == '4');
        setStatus(!ok, ok ? "Data saved." : "Could not save the file.");
        break;
    case '0': return SCREEN_HOME;
    default: setStatus(1, "Unknown action.");
    }
    return SCREEN_FILES;
}

// ---------------------------------------------------------------- entry point

void runDashboard(StoreManager* storeManager) {
    if (!storeManager) return;
    enableTerminalFeatures();
    setStatus(0, "Welcome. Press 6 and then 1 to load system_data.txt, or start adding cars.");
    Screen screen = SCREEN_HOME;
    while (screen != SCREEN_EXIT && !inputEnded) {
        switch (screen) {
        case SCREEN_HOME: screen = homeScreen(storeManager); break;
        case SCREEN_CARS: screen = carsScreen(storeManager); break;
        case SCREEN_PARTS: screen = partsScreen(storeManager); break;
        case SCREEN_CUSTOMERS: screen = customersScreen(storeManager); break;
        case SCREEN_SELL: screen = sellScreen(storeManager); break;
        case SCREEN_GARAGES: screen = garagesScreen(storeManager); break;
        case SCREEN_FILES: screen = filesScreen(storeManager); break;
        default: screen = SCREEN_EXIT;
        }
    }
    clearScreen();
    printf(C_ACCENT "Goodbye." RESET "\n");
}
