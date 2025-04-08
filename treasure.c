#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include "treasure.h"

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

void addTreasure(char *hunt){
    mkdir(hunt, 0777);

    char *dataPath = dataFilepath(hunt);
    int dataFile = open(dataPath, O_RDWR | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR);
    if(dataFile == -1){
        perror("Data File Open Error:");
        close(dataFile);
        free(dataPath);
        exit(-1);
    }

    Treasure tr = {0};
    tr = treasureRead(tr);

    off_t offset = treasureTXT * 2 + sizeof(GPS);
    int checkID = 0;
    int bytesRead = 0;
    
    while(1){
        lseek(dataFile, offset, SEEK_SET);
        bytesRead = read(dataFile, &checkID, sizeof(int));
        if(bytesRead == 0){
            break;
        }
        if(checkID == tr.ID){
            printf("A treasure with this ID already exists.\n");
            tr = treasureRead(tr);
            continue;
        }
        offset = offset + sizeof(Treasure);
    }

    lseek(dataFile, 0, SEEK_END);
    write(dataFile, &tr, sizeof(Treasure));
    close(dataFile);
    free(dataPath);


    char *logPath = logFilepath(hunt);
    int logFile = open(logPath, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if(logFile == -1){
        perror("Log File Open Error:");
        close(logFile);
        free(logPath);
        exit(-1);
    }
   
    char log[100] = {0};
    time_t NOW = time(0);
    struct tm *time_info = localtime(&NOW);
    strftime(log, sizeof(log), "%Y-%m-%d %H:%M:%S", time_info);

    char message[100] = {0};
    sprintf(message," User named %s added the treasure with ID:%d \n",tr.userName, tr.ID);

    write(logFile, &log, strlen(log));
    write(logFile, &message, strlen(message));

    close(logFile);
    free(logPath);
}

char *lastLog(char *hunt){
    char *logPath = logFilepath(hunt);
    int logFile = open(logPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(logFile == -1){
        perror("Log File Open Error:");
        close(logFile);
        free(logPath);
        exit(-1);
    }

    int bytesRead = 0;
    char ch = 0;
    bytesRead = lseek(logFile, 0, SEEK_END);

    while(bytesRead >= 0){
        lseek(logFile, bytesRead, SEEK_SET);
        read(logFile, &ch, sizeof(char));
        if(ch == '\n'){
            break;
        }
        bytesRead = bytesRead - 2;
    }

    char buf[MAX] = {0};
    read(logFile, &buf, MAX);

    char *last = (char*)malloc(strlen(buf)+1);
    strcpy(last, buf);
    last[strlen(last)-1] = '\0';

    free(logPath);
    close(logFile);

    return last;
}

void listTreasures(char *hunt){

    char *dataPath = dataFilepath(hunt);
    int dataFile = open(dataPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(dataFile == -1){
        perror("Data File Open Error:");
        close(dataFile);
        free(dataPath);
        exit(-1);
    }

    Treasure tr = {0};
    int bytesRead = 0;
    bytesRead = lseek(dataFile, 0, SEEK_END);

    printf("HuntName: %s\n", hunt);
    printf("HuntSize: %d Bytes\n", bytesRead);
    printf("Number of Treasures: %ld\n",bytesRead / sizeof(Treasure));
    char *log = lastLog(hunt);
    printf("LastLog: %s\n\n", log);
    
    lseek(dataFile, 0, SEEK_SET);
    while(1){
        
        bytesRead = read(dataFile, &tr.userName, sizeof(tr.userName));
        if(bytesRead == 0){
            break;
        }
        read(dataFile, &tr.clue, sizeof(tr.clue));
        read(dataFile, &tr.gps.x, sizeof(float));
        read(dataFile, &tr.gps.y, sizeof(float));
        read(dataFile, &tr.ID, sizeof(int));
        read(dataFile, &tr.value, sizeof(int));

        treasurePrint(tr);
    }

    free(dataPath);
    close(dataFile);
    free(log);
}

