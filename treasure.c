#include "treasure.h"
#include <stdio.h>


void treasurePrint(Treasure x){
    printf("ID : %d\n", x.ID);
    printf("UserName: %s\n", x.userName);
    printf("Value: %d\n", x.value);
    printf("Clue: %s\n", x.clue);
    printf("Coordinates: %.2f - %.2f\n", x.gps.x, x.gps.y);
}