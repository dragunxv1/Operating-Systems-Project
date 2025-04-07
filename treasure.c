#include "treasure.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

void clearInputBuffer() {
    int c = 0;
    while ((c = getchar()) != '\n' && c!= EOF);
}

Treasure treasureRead(Treasure tres) {
    char buffer[100];

    while(1){
        printf("ID: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &tres.ID) == 1) break;
        printf("Invalid Input for ID - Integer Required\n");
    }

    printf("User Name: ");
    fgets(tres.userName, treasureTXT, stdin);
    if (tres.userName[strlen(tres.userName) - 1] != '\n') {
        clearInputBuffer(); 
    } else {
        tres.userName[strcspn(tres.userName, "\n")] = '\0';
    }

    while(1){
        printf("Value: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &tres.value) == 1) break;
        printf("Invalid Input for Value - Integer Required\n");
    }

    printf("Clue: ");
    fgets(tres.clue, treasureTXT, stdin);
    if (tres.clue[strlen(tres.clue) - 1] != '\n') {
        clearInputBuffer(); 
    } else {
        tres.clue[strcspn(tres.clue, "\n")] = '\0';
    }

    while(1){
        printf("X Coordinates: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%f", &tres.gps.x) == 1) break;
        printf("Invalid Input for Coordinates - Float Required\n");
    }

    while(1){
        printf("Y Coordinates: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%f", &tres.gps.y) == 1) break;
        printf("Invalid Input for Coordinates - Float Required\n");
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

