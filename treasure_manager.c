#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include "treasure.h"



int main(int argc, char **argv){
    char *operation = (char*)malloc(strlen(argv[1]) + 1);
    strncpy(operation, argv[1], strlen(argv[1]));
    operation[strlen(argv[1])] = 0;

    char *huntID = (char*)malloc(strlen(argv[2]) + 1);
    strncpy(huntID, argv[2], strlen(argv[2]));
    huntID[strlen(argv[2])] = 0;

    int opt = determineOperation(operation);



    switch(opt){
        case 1:
            DIR *dir = opendir(huntID);
            if(dir == NULL){
                mkdir(huntID, 0777);
                DIR *dir = opendir(huntID);
            }

            char pathname[50];


            closedir(dir);
            break;
        case 2:
            printf("LIST");
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

    free(operation);
    free(huntID);
    return 0;
}