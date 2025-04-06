#include "treasure.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>


void treasurePrint(Treasure x){
    printf("ID : %d\n", x.ID);
    printf("UserName: %s\n", x.userName);
    printf("Value: %d\n", x.value);
    printf("Clue: %s\n", x.clue);
    printf("Coordinates: %.2f - %.2f\n", x.gps.x, x.gps.y);
}

int determineOperation(char *operation){
    if(strcmp(operation, "--add") == 0){
        return 1;
    }
    else if(strcmp(operation, "--list") == 0){
        return 2;
    }
    else if(strcmp(operation, "--view") == 0){
        return 3;
    }
    else if(strcmp(operation, "--remove_treasure") == 0){
        return 4;
    }
    else if(strcmp(operation, "--remove_hunt") == 0){
        return 5;
    }
    return 0;
}

