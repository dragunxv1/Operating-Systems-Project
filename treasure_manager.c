#include <stdio.h>
#include <stdlib.h>
#include "manager.h"

int main(int argc, char **argv){
    if(argc < 2){
        return -1;
    }
    Operation op = parseOperation(argv[1]);

    switch(op){
        case ADD:
            if(argc < 3){
                printf("Missing argument(s).\n");
                printf("Try: --add <hunt_id>\n");
                return -1;
            } 
            if(addTreasure(argv[2]) != 0){
                printf("Treasure add error.");
                return -1;
            }
            break;
        case LIST:
            if(argc < 3){
                printf("Missing argument(s).\n");
                printf("Try: --list <hunt_id>\n");
                return -1;
            } 
            if(listTreasures(argv[2]) != 0){
                printf("Treasure list error.\n");
                return -1;
            }
            break;
        case VIEW:
            if(argc < 4){
                printf("Missing argument(s).\n");
                printf("Try: --view <hunt_id> <id>\n");
                return -1;
            }
            if(viewTreasure(argv[2], argv[3]) != 0){
                printf("Treasure view error.\n");
                return -1;
            }
            
            break;
        case REMOVE_TREASURE:
            if(argc < 4){
                printf("Missing argument(s).\n");
                printf("Try: --remove_treasure <hunt_id> <id>\n");
                return -1;
            } 
            if(removeTreasure(argv[2], argv[3]) != 0){
                printf("Treasure remove error.");
                return -1;
            }
            break;
        case REMOVE_HUNT:
            if(argc < 3){
                printf("Missing argument(s).\n");
                printf("Try: --remove_hunt <hunt_id>\n");
                return -1;
            }
            if(removeHunt(argv[2]) != 0){
                printf("Hunt remove error.");
                return -1;
            }
            
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