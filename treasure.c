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
    char buffer[MAX];

    while(1){
        printf("ID: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &tres.ID) == 1) break;
        printf("Invalid Input for ID - Integer Required\n");
    }

    printf("UserName: ");
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
    if(filepath == NULL){
        perror("Memory Alloc Error:");
        exit(-1);
    }
    strcat(filepath, hunt);
    strcat(filepath,"/");
    strcat(filepath,"data.bin");
    return filepath;    
}

char* logFilepath(char *hunt){
    char *filepath = (char*)calloc((strlen(hunt) + 20), sizeof(char));
    if(filepath == NULL){
        perror("Memory Alloc Error:");
        exit(-1);
    }
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

int Operation(char *operation){
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
        if(lseek(dataFile, offset, SEEK_SET) == -1){
            perror("LSEEK ERROR:");
            exit(-1);
        }
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

    if(lseek(dataFile, 0, SEEK_END) == -1){
        perror("LSEEK ERROR:");
        exit(-1);
    }
    if(write(dataFile, &tr, sizeof(Treasure)) == -1){
        perror("File write error:");
        exit(-1);
    }
    if(close(dataFile) == -1){
        perror("File close error:");
        exit(-1);
    }

    free(dataPath);

    char message[MAX] = {0};
    sprintf(message,"  Treasure with ID %d was added.\n", tr.ID);

    addLog(hunt, message);
}

void addLog(char *hunt, char *mess){
    char *logPath = logFilepath(hunt);
    int logFile = open(logPath, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if(logFile == -1){
        perror("Log File Open Error:");
        close(logFile);
        free(logPath);
        exit(-1);
    }

    char log[MAX] = {0};
    time_t NOW = time(0);
    struct tm *time_info = localtime(&NOW);
    strftime(log, sizeof(log), "%Y-%m-%d %H:%M:%S", time_info);

    char message[MAX] = {0};
    strncpy(message, mess, MAX);

    if(write(logFile, &log, strlen(log)) == -1){
        perror("File write error:");
        exit(-1);
    }
    if(write(logFile, &message, strlen(message)) == -1){
        perror("File write error:");
        exit(-1);
    }

    if(close(logFile) == -1){
        perror("File close error:");
        exit(-1);
    }


    free(logPath);
}

char *lastLog(char *hunt){
    char *logPath = logFilepath(hunt);
    int logFile = open(logPath, O_RDONLY);
    if(logFile == -1){
        perror("Log File Open Error:");
        close(logFile);
        free(logPath);
        exit(-1);
    }
    int bytesRead = 0;
    char ch = 0;
    bytesRead = lseek(logFile, 0, SEEK_END);
    if(bytesRead == -1){
        perror("LSEEK Error:");
        exit(-1);
    }

    while(bytesRead >= 0){
        if(lseek(logFile, bytesRead, SEEK_SET) == -1){
            perror("LSEEK Error:");
            exit(-1);
        }
        if(read(logFile, &ch, sizeof(char)) == -1){
            perror("File read error:");
            exit(-1);
        }
        if(ch == '\n'){
            break;
        }
        bytesRead = bytesRead - 2;
    }

    char buf[MAX] = {0};
    if(read(logFile, &buf, MAX) == -1){
        perror("File read error :");
        exit(-1);
    }

    char *last = (char*)malloc(strlen(buf)+1);
    if(last == NULL){
        perror("Memory alloc error :");
        exit(-1);
    }
    strcpy(last, buf);
    last[strlen(last)-1] = '\0';

    if(close(logFile) == -1){
        perror("File close error :");
        exit(-1);
    }
    free(logPath);
    return last;
}

void listTreasures(char *hunt){

    char *dataPath = dataFilepath(hunt);
    int dataFile = open(dataPath, O_RDONLY);
    if(dataFile == -1){
        perror("Hunt not found :");
        close(dataFile);
        free(dataPath);
        exit(-1);
    }

    Treasure tr = {0};
    int bytesRead = 0;
    bytesRead = lseek(dataFile, 0, SEEK_END);
    if(bytesRead == -1){
        perror("LSEEK Error :");
        exit(-1);
    }

    printf("HuntName: %s\n", hunt);
    printf("HuntSize: %d Bytes\n", bytesRead);
    printf("Number of Treasures: %ld\n",bytesRead / sizeof(Treasure));
    char *log = lastLog(hunt);
    printf("LastLog: %s\n\n", log);
    
    if(lseek(dataFile, 0, SEEK_SET) == -1){
        perror("LSEEK Error :");
        exit(-1);
    }
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


    if(close(dataFile) == -1){
        perror("File close error :");
        exit(-1);
    }
    free(log);
    free(dataPath);

    char message[MAX] = {0};
    sprintf(message,"  All treasures were listed\n");

    addLog(hunt, message);
}

void viewTreasure(char *hunt, char *treasure){
    char *dataPath = dataFilepath(hunt);
    int dataFile = open(dataPath, O_RDONLY);
    if(dataFile == -1){
        perror("Hunt not found :");
        close(dataFile);
        free(dataPath);
        exit(-1);
    }
    
    int bytesRead = 0;
    Treasure tr = {0};
    int ID = 0;
    int tresID = 0;
    if(sscanf(treasure, "%d", &ID) != 1){
        printf("Invalid treasure ID\n");
        exit(-1);
    }
    off_t offset = sizeof(tr.userName) + sizeof(tr.clue) + sizeof(tr.gps);
    off_t offsecond = offset;

    while(1){
        if(lseek(dataFile, offset, SEEK_SET) == -1){
            perror("LSEEK Error :");
            exit(-1);
        }
        bytesRead = read(dataFile, &tresID, sizeof(int));
        if(bytesRead == 0){
            printf("HuntName: %s\n", hunt);
            printf("This treasure doesn't exist in this hunt.\n");
            free(dataPath);
            close(dataFile);
            exit(-1);
        }

        if(ID == tresID){
            if(lseek(dataFile, offset-offsecond, SEEK_SET) == -1){
                perror("LSEEK Error : ");
                exit(-1);
            }
            read(dataFile, &tr.userName, sizeof(tr.userName));
            read(dataFile, &tr.clue, sizeof(tr.clue));
            read(dataFile, &tr.gps.x, sizeof(float));
            read(dataFile, &tr.gps.y, sizeof(float));
            read(dataFile, &tr.ID, sizeof(int));
            read(dataFile, &tr.value, sizeof(int));
            break;
        }

        offset = offset + sizeof(Treasure);
    }
    printf("HuntName: %s\n", hunt);
    treasurePrint(tr);

    
    if(close(dataFile) == -1){
        perror("File close error :");
        exit(-1);
    }
    free(dataPath);

    char message[MAX] = {0};
    sprintf(message," HUNT %s : Treasure with ID %d was listed.\n", hunt, tr.ID);

    addLog(hunt, message);
}

void removeTreasure(char *hunt, char*treasure){

    char *dataPath = dataFilepath(hunt);
    int dataFile = open(dataPath, O_RDWR);
    if(dataFile == -1){
        perror("Hunt not found :");
        close(dataFile);
        free(dataPath);
        exit(-1);
    }

    Treasure list[MAX];
    int index = 0;
    int x = 0;
    while(1){   
        x = read(dataFile, &list[index], sizeof(Treasure));
        if(x == 0){
            break;
        }
        index++;
    }

    if(close(dataFile) == -1){
        perror("File close error :");
        exit(-1);
    }

    dataFile = open(dataPath, O_RDWR | O_TRUNC);
    if(dataFile == -1){
        perror("Data File Open Error:");
        close(dataFile);
        free(dataPath);
        exit(-1);
    }

    int ID = 0;
    if(sscanf(treasure, "%d", &ID) != 1){
        printf("Invalid treasure ID\n");
        exit(-1);
    }
    int i = 0;
    for(i = 0; i<index; i++){
        if(list[i].ID == ID){
            for(int j = i; j<index; j++){
                list[j] = list[j+1];
            }
            index--;
            break;
        }
    }

    if(lseek(dataFile, 0, SEEK_SET) == -1){
        perror("LSEEK Error :");
        exit(-1);
    }
    for(int i = 0; i<index; i++){
        if(write(dataFile, &list[i], sizeof(Treasure)) == -1){
            perror("File write error :");
            exit(-1);
        }
    }

    free(dataPath);
    if(close(dataFile) == -1){
        perror("File close error :");
        exit(-1);
    }

    char message[100] = {0};
    sprintf(message,"  Treasure with ID %d was deleted.\n", ID);

    addLog(hunt, message);
}

void removeHunt(char *hunt){
    DIR *directory = opendir(hunt);
    if(directory == NULL){
        perror("Opendir Error:");
        exit(-1);
    }

    struct dirent *file;
    char path[pathMAX];
    struct stat statbuf;

    while((file = readdir(directory)) != NULL){
    
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name,"..") == 0){
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", hunt, file->d_name);

        if(stat(path, &statbuf) != 0){
            perror("Stat Error:");
            continue;
        }

        if(S_ISDIR(statbuf.st_mode)){
            removeHunt(path);
        }else{
            remove(path);
        }
    }

    if(closedir(directory) == -1){
        perror("Directory close error :");
        exit(-1);
    }
    if(remove(hunt) == -1){
        perror("File remove error :");
        exit(-1);
    }
}
