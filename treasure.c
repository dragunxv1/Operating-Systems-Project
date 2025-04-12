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

Operation parseOperation(char *argument){
    if(strcmp(argument, "--add") == 0){
        return ADD;
    }
    else if(strcmp(argument, "--list") == 0){
        return LIST;
    }
    else if(strcmp(argument, "--view") == 0){
        return VIEW;
    }
    else if(strcmp(argument, "--remove_treasure") == 0){
        return REMOVE_TREASURE;
    }
    else if(strcmp(argument, "--remove_hunt") == 0){
        return REMOVE_HUNT;
    }
    return 0;
}

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
    char buf[MAX] = {0};
    strncpy(buf,hunt, MAX-allocBUF);
    strcat(buf, "/data.bin");

    char *filepath = (char*)calloc(strlen(buf)+1, sizeof(char));
    if(filepath == NULL){
        perror("Memory Alloc Error:");
        return NULL;
    }
    
    strcpy(filepath, buf);
    return filepath;    
}

char* logFilepath(char *hunt){
    char buf[MAX] = {0};
    strncpy(buf,hunt, MAX-allocBUF);
    strcat(buf, "/logged_hunt.txt");

    char *filepath = (char*)calloc(strlen(buf)+1, sizeof(char));
    if(filepath == NULL){
        perror("Memory Alloc Error:");
        return NULL;
    }
    
    strcpy(filepath, buf);
    return filepath;    
}

void treasurePrint(Treasure* x){
    printf("ID : %d\n", x->ID);
    printf("UserName: %s\n", x->userName);
    printf("Value: %d\n", x->value);
    printf("Clue: %s\n", x->clue);
    printf("Coordinates: %.2f - %.2f\n\n", x->gps.x, x->gps.y);
}

char *linkPath(char *hunt){
    char buf[MAX] = {0};
    strcpy(buf, "logged_hunt-");
    strncat(buf, hunt, MAX-allocBUF);
    strcat(buf,".txt");

    char *logLink = (char*)calloc(strlen(buf)+1, sizeof(char));
    if(logLink == NULL){
        perror("Memory Alloc Error :");
        return NULL;
    }

    strncpy(logLink, buf, strlen(buf));

    return logLink;
}

int addLog(char *hunt, char *mess){
    char *logPath = logFilepath(hunt);
    if(logPath == NULL){
        printf("Log path not found.");
        return -1;
    }
    int logFile = open(logPath, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if(logFile == -1){
        perror("Log File Open Error:");
        close(logFile);
        free(logPath);
        return -1;
    }

    char log[MAX] = {0};
    time_t NOW = time(0);
    struct tm *time_info = localtime(&NOW);
    strftime(log, sizeof(log), "%Y-%m-%d %H:%M:%S", time_info);

    char message[MAX] = {0};
    strncpy(message, mess, MAX);

    if(write(logFile, &log, strlen(log)) == -1){
        perror("File write error:");
        return -1;
    }
    if(write(logFile, &message, strlen(message)) == -1){
        perror("File write error:");
        return -1;
    }

    if(close(logFile) == -1){
        perror("File close error:");
        return -1;
    }

    struct stat buff;
    char *logLink = linkPath(hunt);

    if(stat(logLink, &buff) != 0){
        if(symlink(logPath, logLink) == -1){
            perror("Symlink error :");
            return -1;
        }
    }
    free(logPath);
    free(logLink);
    return 0;
}

int addTreasure(char *hunt){
    mkdir(hunt, 0777);

    char *dataPath = dataFilepath(hunt);
    if(dataPath == NULL){
        printf("Data path not found.");
        return -1;
    }
    int dataFile = open(dataPath, O_RDWR | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR);
    if(dataFile == -1){
        perror("Data File Open Error:");
        close(dataFile);
        free(dataPath);
        return -1;
    }

    free(dataPath);
    Treasure tr = {0};
    tr = treasureRead(tr);

    off_t offset = 0;
    char checkName[treasureTXT] = {0};
    int bytesRead = 0;
    
    while(1){
        if(lseek(dataFile, offset, SEEK_SET) == -1){
            perror("LSEEK ERROR:");
            return -1;
        }
        bytesRead = read(dataFile, &checkName, treasureTXT);
        if(bytesRead == 0){
            break;
        }
        if(strcmp(tr.userName, checkName) == 0){
            printf("A treasure with this UserName already exists.\n");
            tr = treasureRead(tr);
            continue;
        }
        offset = offset + sizeof(Treasure);
    }

    if(lseek(dataFile, 0, SEEK_END) == -1){
        perror("LSEEK ERROR:");
        return -1;
    }
    if(write(dataFile, &tr, sizeof(Treasure)) == -1){
        perror("File write error:");
        return -1;
    }
    if(close(dataFile) == -1){
        perror("File close error:");
        return -1;
    }

    char message[MAX] = {0};
    sprintf(message,"  Treasure with ID %d was added.\n", tr.ID);

    if(addLog(hunt, message) != 0){
        printf("Log add error.");
        return -1;
    }

    return 0;
}

char *lastLog(char *hunt){
    char *logPath = logFilepath(hunt);
    if(logPath == NULL){
        printf("Log Path not found.");
        return NULL;
    }
    int logFile = open(logPath, O_RDONLY);
    if(logFile == -1){
        perror("Log File Open Error:");
        close(logFile);
        free(logPath);
        return NULL;
    }

    free(logPath);
    int bytesRead = 0;
    char ch = 0;
    bytesRead = lseek(logFile, 0, SEEK_END);
    if(bytesRead == -1){
        perror("LSEEK Error:");
        return NULL;
    }

    while(bytesRead >= 0){
        if(lseek(logFile, bytesRead, SEEK_SET) == -1){
            perror("LSEEK Error:");
            return NULL;
        }
        if(read(logFile, &ch, sizeof(char)) == -1){
            perror("File read error:");
            return NULL;
        }
        if(ch == '\n'){
            break;
        }
        bytesRead = bytesRead - 2;
    }

    char buf[MAX] = {0};
    if(read(logFile, &buf, MAX) == -1){
        perror("File read error :");
        return NULL;
    }

    char *last = (char*)malloc(strlen(buf)+1);
    if(last == NULL){
        perror("Memory alloc error :");
        return NULL;
    }
    strcpy(last, buf);
    last[strlen(last)-1] = '\0';

    if(close(logFile) == -1){
        perror("File close error :");
        return NULL;
    }
    
    return last;
}

int listTreasures(char *hunt){

    char *dataPath = dataFilepath(hunt);
    if(dataPath == NULL){
        printf("Data Path not found.");
        return -1;
    }
    int dataFile = open(dataPath, O_RDONLY);
    if(dataFile == -1){
        perror("Hunt not found :");
        close(dataFile);
        free(dataPath);
        return -1;
    }

    free(dataPath);
    Treasure tr = {0};
    int bytesRead = 0;
    bytesRead = lseek(dataFile, 0, SEEK_END);
    if(bytesRead == -1){
        perror("LSEEK Error :");
        return -1;
    }

    printf("HuntName: %s\n", hunt);
    printf("HuntSize: %d Bytes\n", bytesRead);
    printf("Number of Treasures: %ld\n",bytesRead / sizeof(Treasure));
    char *log = lastLog(hunt);
    if(log == NULL){
        printf("Last log not found.");
        return -1;
    }
    printf("LastLog: %s\n\n", log);
    free(log);
    
    if(lseek(dataFile, 0, SEEK_SET) == -1){
        perror("LSEEK Error :");
        exit(-1);
    }
    while(1){
        
        bytesRead = read(dataFile, &tr, sizeof(tr));
        if(bytesRead != sizeof(tr)){
            break;
        }
        treasurePrint(&tr);
    }

    if(close(dataFile) == -1){
        perror("File close error :");
        return -1;
    }
    
    char message[MAX] = {0};
    sprintf(message,"  All treasures were listed\n");

    if(addLog(hunt, message) != 0){
        printf("Log add error.");
        return -1;
    }

    return 0;
}

int viewTreasure(char *hunt, char *treasure){
    char *dataPath = dataFilepath(hunt);
    if(dataPath == NULL){
        printf("Data Path not found.");
        return -1;
    }
    int dataFile = open(dataPath, O_RDONLY);
    if(dataFile == -1){
        perror("Hunt not found :");
        close(dataFile);
        free(dataPath);
        return -1;
    }

    int bytesRead = 0;
    Treasure tr = {0};
    int ID = 0;
    if(sscanf(treasure, "%d", &ID) != 1){
        printf("Treasure read error :");
        return -1;
    }

    off_t offset = 0;

    while(1){
        if(lseek(dataFile, offset, SEEK_SET) == -1){
            perror("LSEEK Error :");
            return -1;
        }
        bytesRead = read(dataFile, &tr, sizeof(tr));
        if(bytesRead != sizeof(tr)){
            printf("HuntName: %s\n", hunt);
            printf("This treasure doesn't exist in this hunt.\n");
            free(dataPath);
            close(dataFile);
            return -1;
        }

        if(ID == tr.ID){
            printf("HuntName: %s\n", hunt);
            treasurePrint(&tr);

            if(close(dataFile) == -1){
                perror("File close error :");
                return -1;
            }

            char message[MAX] = {0};
            sprintf(message," HUNT %s : Treasure with ID %d was listed.\n", hunt, tr.ID);

            if(addLog(hunt, message) != 0){
                printf("Log add error.");
                return -1;
            }
            break;
        }else{
            offset = offset + sizeof(Treasure);
        }
    }
    free(dataPath);
    return 0;
}

int removeTreasure(char *hunt, char*treasure){

    char *dataPath = dataFilepath(hunt);
    if(dataPath == NULL){
        printf("Data Path not found.");
        return -1;
    }
    int dataFile = open(dataPath, O_RDWR);
    if(dataFile == -1){
        perror("Hunt not found :");
        close(dataFile);
        free(dataPath);
        return -1;
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
        return -1;
    }

    dataFile = open(dataPath, O_RDWR | O_TRUNC);
    if(dataFile == -1){
        perror("Data File Open Error:");
        close(dataFile);
        free(dataPath);
        return -1;
    }

    int ID = 0;
    if(sscanf(treasure, "%d", &ID) != 1){
        printf("Invalid treasure ID\n");
        return -1;
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
        return -1;
    }
    for(int i = 0; i<index; i++){
        if(write(dataFile, &list[i], sizeof(Treasure)) == -1){
            perror("File write error :");
            return -1;
        }
    }

    free(dataPath);
    if(close(dataFile) == -1){
        perror("File close error :");
        return -1;
    }

    char message[100] = {0};
    sprintf(message,"  Treasure with ID %d was deleted.\n", ID);

    if(addLog(hunt, message) != 0){
        printf("Log add error.");
        return -1;
    }

    return 0;
}

int removeHunt(char *hunt){
    DIR *directory = opendir(hunt);
    if(directory == NULL){
        perror("Hunt not found :");
        return -1;
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
        return -1;
    }
    if(remove(hunt) == -1){
        perror("File remove error :");
        return -1;
    }

    char *logLink = linkPath(hunt);

    remove(logLink);
    free(logLink);

    return 0;
}
