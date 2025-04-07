#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "treasure.h"



int main(int argc, char **argv){
    int opt = determineOperation(argv[1]);

    mkdir(argv[2], 0777);

    char *dataPath = dataFilepath(argv[2]);
    int dataFile = open(dataPath, O_RDWR | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR);
    if(dataFile == -1){
        perror("Data File Open Error:");
        close(dataFile);
        free(dataPath);
        exit(-1);
    }

    char *logPath = logFilepath(argv[2]);
    int logFile = open(logPath, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if(logFile == -1){
        perror("Log File Open Error:");
        close(dataFile);
        close(logFile);
        free(dataPath);
        free(logPath);
        exit(-1);
    }

    switch(opt){
        case 1:
            Treasure tr = {0};
            tr = treasureRead(tr);
            
            write(dataFile, &tr, sizeof(Treasure));
           
            char log[100] = {0};
            time_t NOW = time(0);
            struct tm *time_info = localtime(&NOW);
            strftime(log, sizeof(log), "%Y-%m-%d %H:%M:%S", time_info);

            char message[100] = {0};
            sprintf(message," User named %s added the treasure with ID:%d \n",tr.userName, tr.ID);


            write(logFile, &log, strlen(log));
            write(logFile, &message, strlen(message));
            break;

        case 2:
            printf("Hunt name: %s\n", argv[2]);
            Treasure list[MAX];
            int treasureCounter = 0;
            while(read(dataFile, &list[treasureCounter], sizeof(Treasure)) != 0){
                treasureCounter++;
            }
            printf("Total treasures: %d\nTotal file size %ld BYTES\n", treasureCounter, sizeof(Treasure) * treasureCounter);

            for(int i = 0; i<treasureCounter; i++){
                treasurePrint(list[i]);
            }

            break;
        case 3:
            printf("VIEW");
            break;
        case 4:
            printf("REMOVE");
            break;
        case 5:
            printf("REMOVEF");
            break;
        default :
            printf("Incorrect Option!\n");
            printf("Available options:\n");
            printf("--add <hunt_id> (add a new treasure to the specified hunt)\n");
            printf("--list <hunt_id> (list ll treasures to the specified hunt)\n");
            printf("--view <hunt_id> <id> (view details about a specifi treasure)\n");
            printf("--remove_treasure <hunt_id> <id> (remove a treasure)\n");
            printf("--remove_hunt <hunt_id> (remove an entire hunt)\n");
    }


    close(dataFile);
    close(logFile);
    free(dataPath);
    free(logPath);

    return 0;
}