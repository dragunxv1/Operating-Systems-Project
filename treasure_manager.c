#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include "treasure.h"

int main(int argc, char **argv){
    int opt = determineOperation(argv[1]);


    switch(opt){
        case 1:
            addTreasure(argv[2]);
            break;
        case 2:
            listTreasures(argv[2]);
            break;
        case 3:
            viewTreasure(argv[2], argv[3]);
            break;
        case 4:
            removeTreasure(argv[2], argv[3]);
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


  
    return 0;
}