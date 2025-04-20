#include <stdio.h>
#include <errno.h>
#include "hub.h"

int main(void){
    int x = 0;
    COMMAND comm = 0;

    while(1){
        printf("Choose a command:\n");
        printf("1) Start Monitor.\n");
        printf("2) List Hunts.\n");
        printf("3) List Treasures.\n");
        printf("4) View Treasure.\n");
        printf("5) Stop Monitor.\n");
        printf("0) EXIT.\n");

        if(scanf("%d", &x) != 1){
            perror("Menu Scanf :");
            return -1;
        }

        comm = x;

        switch(comm){
            case start_monitor:
            printf("START MONITOR\n");
            break;
            case list_hunts:
            printf("LIST HUNTS\n");
            break;
            case list_treasures:
            printf("LIST TREASURES\n");
            break;
            case view_treasures:
            printf("VIEW TREASURES\n");
            break;
            case stop_monitor:
            printf("STOP MONITOR\n");
            break;
            case exit:
            return 0;
            default:
            printf("Incorrect Command. Try again.\n");
        }

    }
    return 0;
}