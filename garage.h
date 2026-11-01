#ifndef GARAGE_H
#define GARAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"


typedef struct {
    int id;
    char name[MAX_GARAGE_NAME_LEN];
    char address[MAX_GARAGE_ADDRESS_LEN];
} Garage;


typedef struct GarageNode {
    Garage garage;
    struct GarageNode* next;
} GarageNode;


typedef struct {
    GarageNode* head;
} GarageList;


GarageList* createGarageList();
int addGarage(GarageList* list, Garage garage);
void printGarages(const GarageList* list);
int removeGarage(GarageList* list, int id);
void appendGarageNode(GarageList* list, GarageNode* node);
void clearGarageList(GarageList* list);

#endif // GARAGE_H
