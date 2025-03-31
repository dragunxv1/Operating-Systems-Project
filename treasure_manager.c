#include <stdio.h>
#include <string.h>
#include "treasure.h"



int main(int argc, char **argv){
    Treasure x;
    x.ID = 12;
    x.value = 500;
    strcpy(x.userName, "John");
    strcpy(x.clue, "left-right");
    x.gps.x = 120.3;
    x.gps.y = 150.4;

    treasurePrint(x);
    return 0;
}