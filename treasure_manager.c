#include <stdio.h>
#include "treasure.h"

int main(int argc, char **argv){
    int opt = Operation(argv[1]);


    switch(opt){
        case 1:
            if(argv[2] != NULL){
                addTreasure(argv[2]);
            }
            else{
                printf("Missing argument(s).\n");
                printf("Try: --add <hunt_id>\n");
            }   
            break;
        case 2:
            if(argv[2] != NULL){
                listTreasures(argv[2]);
            }
            else{
                printf("Missing argument(s).\n");
                printf("Try: --list <hunt_id>\n");
            }    
            break;
        case 3:
            if(argv[2] != NULL && argv[3] != NULL){
                viewTreasure(argv[2], argv[3]);
            }
            else{
                printf("Missing argument(s).\n");
                printf("Try: --view <hunt_id> <id>\n");
            } 
            break;
        case 4:
            if(argv[2] != NULL && argv[3] != NULL){
                removeTreasure(argv[2], argv[3]);
            }
            else{
                printf("Missing argument(s).\n");
                printf("Try: --remove_treasure <hunt_id> <id>\n");
            } 
            break;
        case 5:
            if(argv[2] != NULL){
                removeHunt(argv[2]);
            }
            else{
                printf("Missing argument(s).\n");
                printf("Try: --remove_hunt <hunt_id>\n");
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