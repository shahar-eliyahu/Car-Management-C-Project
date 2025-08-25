#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include "store_manager.h"
#include "constants.h"

// Function prototypes
int saveToFile(const char* filename, const StoreManager* store, int isBinary);
int loadFromFile(const char* filename, StoreManager* store, int isBinary);
void saveCompressedPartToFile(FILE* file, const Part* part);
Part* loadCompressedPartFromFile(FILE* file);

#endif // FILE_OPERATIONS_H