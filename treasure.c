#include "treasure.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

void clearInputBuffer() {
    while (getchar() != '\n');
}

Treasure treasureRead(Treasure tres){

    int validInput = 0;
    printf("ID:");
    while(validInput != 1){
        if(scanf("%d", &tres.ID) == 1){
            validInput = 1;
        }else{
            printf("Invalid Input for ID - Integer Required\n");
            printf("ID:");
            clearInputBuffer();
        }
    }

    printf("User Name:");
    fgets(tres.userName, treasureTXT, stdin);
    tres.userName[strcspn(tres.userName, "\n")] = '\0';
    clearInputBuffer();

    validInput = 0;
    printf("Value:");
    while(validInput != 1){
        if(scanf("%d", &tres.value) == 1){
            validInput = 1;
        }else{
            printf("Invalid Input for Value - Integer Required\n");
            printf("Value:");
            clearInputBuffer();
        }
    }

    printf("Clue:");
    fgets(tres.clue, treasureTXT, stdin);
    tres.clue[strcspn(tres.clue, "\n")] = '\0';
    clearInputBuffer();
   

    validInput = 0;
    printf("X Coordinates:");
    while(validInput != 1){
        if(scanf("%f", &tres.gps.x) == 1){
            validInput = 1;
        }else{
            printf("Invalid Input for Coordinates - Float Required\n");
            printf("X Coordinates:");
            clearInputBuffer();
        }
    }

    validInput = 0;
    printf("Y Coordinates:");
    while(validInput != 1){
        if(scanf("%f", &tres.gps.y) == 1){
            validInput = 1;
        }else{
            printf("Invalid Input for Coordinates - Float Required\n");
            printf("Y Coordinates:");
            clearInputBuffer();
        }
    }
    
    return tres;
}

char* dataFilepath(char *hunt){
    char *filepath = (char*)calloc((strlen(hunt) + 20), sizeof(char));
    strcat(filepath, hunt);
    strcat(filepath,"/");
    strcat(filepath,"data.bin");
    return filepath;    
}

char* logFilepath(char *hunt){
    char *filepath = (char*)calloc((strlen(hunt) + 20), sizeof(char));
    strcat(filepath, hunt);
    strcat(filepath,"/");
    strcat(filepath,"logged_hunt.txt");
    return filepath;    
}

void treasurePrint(Treasure x){
    printf("ID : %d\n", x.ID);
    printf("UserName: %s\n", x.userName);
    printf("Value: %d\n", x.value);
    printf("Clue: %s\n", x.clue);
    printf("Coordinates: %.2f - %.2f\n\n", x.gps.x, x.gps.y);
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

